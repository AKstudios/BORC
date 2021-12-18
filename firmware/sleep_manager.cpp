#include "globals.h"
#include "LowPower.h"

////=========================================================================================================
//// WDT_vect - Interrupt Service Routine for the watchdog timer. This needs to exist for the timer
////=========================================================================================================
//ISR (WDT_vect) {wdt_disable();}  // disable watchdog
////=========================================================================================================


//=========================================================================================================
// init() - setup sleep stuff
//=========================================================================================================
void CSleepMgr::init()
{   
    // start sleep timer
    start_timer();
}
//=========================================================================================================


//=========================================================================================================
// execute() - checks timer and goes to sleep
//=========================================================================================================
void CSleepMgr::execute()
{   
    // if it's not time to sleep, don't do anything
    if (!m_sleep_timer.is_expired()) return;

    // if it's time to sleep and we're in manual mode...
    if (ee.run_mode == MANUAL_MODE)
    {   
        // ... move the motor
        Servo.move_to_index(ee.manual_index);
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
                wakeup_from_knob();
                return;
            }
        }

        // radio, servo, blah
        wakeup_from_timer();
    }
}
//=========================================================================================================


//=========================================================================================================
// start_timer() - starts sleep timer
//=========================================================================================================
void CSleepMgr::start_timer(int timeout_ms)
{   Serial.println("start sleep timer");
    m_sleep_timer.start(timeout_ms);
}
//=========================================================================================================


//--------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::wakeup_from_timer()
{
    // turn power on to all devices
    PowerMgr.powerOnAll();

    // read current temperature
    TempHum.read_temp();

    // if we're in setpoint mode..
    if (ee.run_mode == SETPOINT_MODE)
    {   
        // reinitialize the servo driver
        Servo.reinit();

        // compute new position for servo
        int new_position = int(PID.compute(TempHum.temp, 32));
        
        Serial.print("Temp: ");
        Serial.println(c_to_f(TempHum.temp));
        Serial.print("Setpoint: ");
        Serial.println(ee.setpoint_f);
        Serial.print("New servo position: ");
        Serial.println(new_position);
        Serial.println();

        // move the servo to new position
        Servo.move_to_position(new_position);

        // to-do:
        // if servo already at position, don't move
        // flip servo position to proper open/close
        // fix integral windup
    }


    // send data via radio - t, rh, setpoint, debug

}
//=========================================================================================================


//--------------------------------------------------------------------------------------------------------
// on_knob_activity() - this gets called anytime there is activity on the knob
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::on_knob_activity()
{ 
    m_sleep_timer.kick();

    m_wakeup_from_knob = true;
}
//=========================================================================================================

//--------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::wakeup_from_knob()
{
    Knob.throw_away_next_event();
    PowerMgr.powerOnAll();
    Servo.reinit();
    System.return_to_run_mode();
    start_timer();
}
//=========================================================================================================


//=========================================================================================================
// kick_timer() - football
//=========================================================================================================
void CSleepMgr::kick_timer() {m_sleep_timer.kick();}
//=========================================================================================================