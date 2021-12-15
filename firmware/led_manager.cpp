#include "globals.h"

//=========================================================================================================
// init() - set LED pins as outputs
//=========================================================================================================
void CLedMgr::init()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
}
//=========================================================================================================


//=========================================================================================================
// off() - turn LED pins off
//=========================================================================================================
void CLedMgr::off()
{
    digitalWrite(RED_PIN, LOW);
    digitalWrite(GREEN_PIN, LOW);
    digitalWrite(BLUE_PIN, LOW);
    
    m_state = false;
}
//=========================================================================================================


//=========================================================================================================
// on() - turn LED pins on
//=========================================================================================================
void CLedMgr::on(led_color_t pattern)
{
    if (pattern & RED)  digitalWrite(RED_PIN, HIGH);
    if (pattern & GREEN)  digitalWrite(GREEN_PIN, HIGH);
    if (pattern & BLUE)  digitalWrite(BLUE_PIN, HIGH);

    m_state = (pattern != OFF);
}
//=========================================================================================================


//=========================================================================================================
// set() - Call this to set the LED to a new pattern
// If period_ms = 0, LED is solid
// If period_ms = 1, LED does a single 10ms flash
// If period_ms is anything else, it's the blink period.    
//=========================================================================================================
void CLedMgr::set(led_color_t color, unsigned int period_ms, bool single_flash)
{
    // save this for posterity
    m_color = color & 7;

    // save this for posterity
    m_is_single_flash = single_flash;

    // kill the timer to change to a new pattern
    m_timer.stop();

    // turn off all LED pins that might be on
    off();

    // turn on the pins the user asked for
    on(color);

    // if we're blinking, start the timer
    if (period_ms)  m_timer.start(period_ms);
}
//=========================================================================================================


//=========================================================================================================
// execute() - Call this periodicially to allow the state machine to do its thing
//=========================================================================================================
void CLedMgr::execute()
{
    // timer not yet expired, do nothing
    if (!m_timer.is_expired())  return;

    // if it's on, turn it off; if it's off, turn it on
    if (m_state)
        off();
    else
        on(m_color);
    
    // if we're doing a single flash, stop the timer 
    if (m_is_single_flash)   m_timer.stop();
}
//=========================================================================================================
