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
    if (ee.run_mode == MANUAL)
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

            // do temp/rh
            // PID stuff
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
{   Serial.println("start timer");
    m_sleep_timer.start(timeout_ms);
}
//=========================================================================================================


//--------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::wakeup_from_timer()
{
    Serial.println("wake up from timer");
    delay(10);
    // take temp/rh measurement

    // in auto mode - compute new setpoint for PID and drive the motor there
    // in manual mode - do nothing

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