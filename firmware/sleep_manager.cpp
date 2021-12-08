#include "globals.h"

#include "LowPower.h"
//#include <avr/interrupts.h>

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
    if (System.mode == MANUAL)
    {   
        // ... move the motor
        Servo.move_to_index(System.manual_index);
    }

    // power down necessary devices here to reduce sleep current draw
    PowerMgr.powerOff(CURRENT_SENSE_POWER_PIN);
    PowerMgr.powerOff(LED_SCREEN_POWER_PIN);
    PowerMgr.powerOff(TEMP_SENSOR_POWER_PIN);
    PowerMgr.powerOff(DRIVER_POWER_PIN);
    PowerMgr.powerOff(SERVO_POWER_PIN);

    // put the radio and SPI flash to sleep
    // radio.sleep();
    // flash.sleep();

    // turn RGB LED off just to be sure
    // toggleLED(0,0,0);
    
    
    m_wakeup_from_knob = false;

    while (true)
    {
        for (int i=0; i<4; i++)
        {
            LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
            
            if (m_wakeup_from_knob) break;

            // do temp/rh
            // PID stuff
        }

        if (m_wakeup_from_knob) break;

        // radio, servo, blah
        wakeup_from_timer();
    }

    wakeup_from_knob();

}
//=========================================================================================================


//=========================================================================================================
// start_timer() - starts sleep timer
//=========================================================================================================
void CSleepMgr::start_timer(int timeout_ms=5000)
{
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
//
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::signal_wakeup()
{ 

    m_wakeup_from_knob = true;
}
//=========================================================================================================

//--------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------
void CSleepMgr::wakeup_from_knob()
{
    Knob.throw_away_next_event();
    PowerMgr.powerI2C();
    Servo.reinit();
    ManualModeMgr.start();
}
//=========================================================================================================