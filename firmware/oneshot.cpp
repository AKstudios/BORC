#include "oneshot.h"
#include "Arduino.h"  // needs to be included to use millis in a custom class


//=========================================================================================================
// start() - Starts or restarts the timer
//=========================================================================================================
void OneShot::start(unsigned int duration_ms)
{
    m_duration_ms = duration_ms;
    m_start_time  = millis();
    m_is_running  = true;
}
//=========================================================================================================


//=========================================================================================================
// is_expired() - Returns true if our one-shot timer was running and has expired
//=========================================================================================================
bool OneShot::is_expired()
{
    // A timer that isn't running is by definition not expired
    if (!m_is_running) return false;

    // How many milliseconds have elapsed since the timer was started?
    unsigned long elapsed = millis() - m_start_time;

    // Is the timer expired?
    bool expired = elapsed > m_duration_ms;

    // If the timer is expired, stop the timer
    if (expired) m_is_running = false;

    // Tell the caller whether or not this one-shot timer is expired
    return expired;
}
//=========================================================================================================
