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

        // Tell the temperature controller what its setpoint is
        TempCtrl.new_setpoint_f(ee.setpoint_f);

        // If it's time simulate the system waking up from the timer, do so
        if (!m_ctrl_timer.is_running()) m_ctrl_timer.start(SLEEP_TIME_SECS * 1000); 

        // And restart the sleep timer
        start_sleep_timer();
    }

    // This is a repeating timer.  If it's expired, simulate waking from sleep
    if (m_ctrl_timer.is_expired()) on_wakeup_from_timer();

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
    Radio.sleep();
    Flash.sleep();

    // write data to EEPROM
    EEPROM.write();

    // Tell the temperature controller what its setpoint is
    TempCtrl.new_setpoint_f(ee.setpoint_f);

    Serial.println("sleep");
    delay(10);

    // When this flag is set to true by knob activity, it's time to wake up!
    m_wakeup_from_knob = false;

    // stay in a sleep loop unless someone interacts with the knob
    while (true)
    {
        for (int i=0; i<4; i++)
        {   
            // deep sleep for 8 seconds
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

            // if battery is low, flash red LED
            if (Battery.is_low())
            {
                Led.set(RED, 0);
                delay(10);
                Led.set(OFF, 0);
            }
                
            // if someone interacts with knob, wake up here
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

    // !! VERY IMPORTANT !! - wake up flash after sleep to prevent the device from hanging up!
    // this is needed before putting flash to sleep again. Calling Flash.sleep() twice without calling
    // Flash.wakeup() will hang the device as per the flash's datasheet.
    Flash.wakeup();

    // Read the current temperature & humidity
    bool sensor_ok = SHT31.read_f(&System.temp_f, &System.hum);

    // if we're in setpoint mode, run the temp controller and the servo
    if (ee.run_mode == SETPOINT_MODE)
    {
        // reinitialize the servo driver
        Servo.reinit();
        
        // compute new position for servo
        if (sensor_ok && TempCtrl.compute(System.temp_f, SLEEP_TIME_SECS, &new_position))
        {
            Serial.print("Temp F: ");
            Serial.println(strfloat(System.temp_f, 0, 2));
            Serial.print("Setpoint: ");
            Serial.println(strfloat(ee.setpoint_f, 0, 2));
            Serial.print("New servo position: ");
            Serial.println(new_position);
            Serial.println();

            // Now move the servo to new position
            Servo.move_to_pwm(new_position, 4000, true);

            // Tell awake_mode_execute() that it has no idea where the servo is now
            m_last_driven_index = 0xFF;
        }
    }

    // if radio works properly
    if (!(System.error_byte & RADIO_ERR))
    {
        // Transmit a packet of telemetry data back to the gateway
        Radio.transmit_telemetry();
        // Radio.transmit_config(3);
    
        // put the radio to sleep
        Radio.sleep();
    }

    // if SPI flash works properly
    if (!(System.error_byte & FLASH_ERR))
        Flash.sleep();

    // Only power down all devices in sleep mode to reduce sleep current draw
    if (m_mode != AWAKE_MODE)
        PowerMgr.powerOffAll();
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
    Display.reinit();
    Servo.reinit();
    System.return_to_run_mode();
    start_sleep_timer();

    // !! VERY IMPORTANT !! - wake up flash after sleep to prevent the device from hanging up!
    // this is needed before putting flash to sleep again. Calling Flash.sleep() twice without calling
    // Flash.wakeup() will hang the device as per the flash's datasheet.
    Flash.wakeup();
}
//=========================================================================================================


//=========================================================================================================
// kick_sleep_timer() - football
//=========================================================================================================
void CSleepMgr::kick_sleep_timer() {m_sleep_timer.kick();}
//=========================================================================================================

