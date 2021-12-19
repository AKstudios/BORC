#include "globals.h"
#include "LowPower.h"

////=========================================================================================================
//// WDT_vect - Interrupt Service Routine for the watchdog timer. This needs to exist for the timer
////=========================================================================================================
//ISR (WDT_vect) {wdt_disable();}  // disable watchdog
////=========================================================================================================



//=========================================================================================================
// start_sleep_mode() - Sets us up to automatically to go sleep a few seconds from now
//=========================================================================================================
void CSleepMgr::start_sleep_mode()
{
    m_mode = SLEEP_MODE;
    start_sleep_timer();
}
//=========================================================================================================

//=========================================================================================================
// start_awake_mode() - Sets us up to simulate the behavior of sleep mode without ever actually sleeping
//=========================================================================================================
void CSleepMgr::start_awake_mode()
{
    // We're staying awake until further notice
    m_mode = AWAKE_MODE;

    // We don't know where we last parked the servo
    m_last_driven_index = 0xFF;

    // We're going to simulate going to sleep a few seconds from now
    start_sleep_timer();

    // And start a timer that expires every 32 seconds to we can drive the servo 
    m_pid_timer.start(32000);
}
//=========================================================================================================



//=========================================================================================================
// execute_awake_mode() - Manages timer related events when we're in AWAKE_MODE
//=========================================================================================================
void CSleepMgr::execute_awake_mode()
{
    // If it's time to simulate going to sleep...
    if (m_sleep_timer.is_expired())
    {
        // Save the dirty EEPROM structure to physical EEPROM
        EEPROM.write();

        // If we're in manual mode, move the servo to the correct position
        if (ee.run_mode == MANUAL_MODE && ee.manual_index != m_last_driven_index) 
        {
            Servo.move_to_index(ee.manual_index);
            m_last_driven_index = ee.manual_index;
        }

        // Tell the temperature controll what its setpoint is
        TempCtrl.new_setpoint_f(ee.setpoint_f);

        // If it's time simulate the system waking up from the timer, do so
        if (!m_pid_timer.is_running()) m_pid_timer.start(32000);

        // And restart the sleep timer
        start_sleep_timer();
    }

    // This is a repeating timer.  If it's expired, simulate waking from sleep
    if (m_pid_timer.is_expired()) on_wakeup_from_timer();

}
//=========================================================================================================



//=========================================================================================================
// execute() - checks timer and goes to sleep
//=========================================================================================================
void CSleepMgr::execute()
{
    if (m_mode == AWAKE_MODE)
        execute_awake_mode();
    else
        execute_sleep_mode();
}
//=========================================================================================================


//=========================================================================================================
// execute() - checks timer and goes to sleep
//=========================================================================================================
void CSleepMgr::execute_sleep_mode()
{
    // if it's not time to sleep, don't do anything
    if (!m_sleep_timer.is_expired()) return;

    // if it's time to sleep and we're in manual mode...
    if (ee.run_mode == MANUAL_MODE)
    {   
        Servo.move_to_index(ee.manual_index);
        m_last_driven_index = ee.manual_index;
    }

    // clear display
    Display.clear();

    // turn RGB LED off
    Led.set(OFF);

    // power down all devices here to reduce sleep current draw
    PowerMgr.powerOffAll();

    // put the radio and SPI flash to sleep
    // radio.sleep();
    // flash.sleep();

    // write data to EEPROM
    EEPROM.write();

    // set knob wakeup flag to false before going to sleep
    m_wakeup_from_knob = false;

    // Tell the temperature control what its setpoint is
    TempCtrl.new_setpoint_f(ee.setpoint_f);

    Serial.println("sleep");
    delay(10);
    
    // stay in a sleep loop unless someone interacts with the knob
    while (true)
    {
        for (int i=0; i<4; i++)
        {
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            
            if (m_wakeup_from_knob)
            {
                on_wakeup_from_knob();
                return;
            }
        }

        // radio, servo, blah
        on_wakeup_from_timer();
    }
}
//=========================================================================================================



//=========================================================================================================
// start_sleep_timer() - starts sleep timer
//=========================================================================================================
void CSleepMgr::start_sleep_timer(int timeout_ms)
{
    m_sleep_timer.start(timeout_ms);
}
//=========================================================================================================



//=========================================================================================================
// on_wakeup_from_timer() - Called when the system wakes from sleep due to the timer
//=========================================================================================================
void CSleepMgr::on_wakeup_from_timer()
{
    nc_out_t new_position;

    // turn power on to all devices
    PowerMgr.powerOnAll();

    // if we're in setpoint mode, run the PID controller and the servo
    if (ee.run_mode == SETPOINT_MODE)
    {
        // reinitialize the servo driver
        Servo.reinit();

        // Read the current temperature
        float temp_c = TempHum.read_temp_c();

        // compute new position for servo
        if (TempCtrl.compute(temp_c, 32, &new_position))
        {
            // move the servo to new position
            Servo.move_to_pwm(new_position, 4000, true);

            Serial.print("Temp C: ");
            Serial.println(strfloat(temp_c, 0, 2));
            Serial.print("Setpoint: ");
            Serial.print(strfloat(f_to_c(ee.setpoint_f), 0, 2));
            Serial.print("  ");
            Serial.println(ee.setpoint_f);
            Serial.print("New servo position: ");
            Serial.println(new_position);
            Serial.println();

            // Tell awake_mode_execute() that it has no idea where the servo is now
            m_last_driven_index = 0xFF;
        }

    }

    // send data via radio - t, rh, setpoint, debug

}
//=========================================================================================================



//=========================================================================================================
// on_knob_activity() - this gets called anytime there is activity on the knob
//=========================================================================================================
void CSleepMgr::on_knob_activity()
{ 
    m_sleep_timer.kick();
    m_wakeup_from_knob = true;
}
//=========================================================================================================


//=========================================================================================================
// on_wakeup_from_knob() - Called when the system is awoken from sleep by activity on the rotary kob
//=========================================================================================================
void CSleepMgr::on_wakeup_from_knob()
{
    Knob.throw_away_next_event();
    PowerMgr.powerOnAll();
    Servo.reinit();
    System.return_to_run_mode();
    start_sleep_timer();
}
//=========================================================================================================


//=========================================================================================================
// kick_sleep_timer() - football
//=========================================================================================================
void CSleepMgr::kick_sleep_timer() {m_sleep_timer.kick();}
//=========================================================================================================