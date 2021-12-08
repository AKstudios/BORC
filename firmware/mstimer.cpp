#include "mstimer.h"
#include "Arduino.h"  


//=========================================================================================================
// start() - Starts or restarts the timer
//=========================================================================================================
void msTimer::start(unsigned int duration_ms)
{
    m_duration_ms = duration_ms;
    m_start_time  = millis();
    m_is_running  = true;
}
//=========================================================================================================


//=========================================================================================================
// is_expired() - Returns true if our timer was running and has expired
//=========================================================================================================
bool msTimer::is_expired()
{
    // A timer that isn't running is by definition not expired
    if (!m_is_running) return false;

    // Fetch the current timestamp
    unsigned long now = millis();

    // How many milliseconds have elapsed since the timer was started?
    unsigned long elapsed = now - m_start_time;

    // Is the timer expired?
    bool expired = elapsed > m_duration_ms;

    // If the timer hasn't yet expired, tell the caller
    if (!expired) return false;

    //---------------------------------------------------
    // If we get here, we're restarting an expired timer
    //---------------------------------------------------

    // Compute the timestamp when the timer restarted
    m_start_time += m_duration_ms;
            
    // Compute the number of milliseconds that have elapsed since the new start time
    elapsed = now - m_start_time;

    // If the restarted timer is <already> expired, it restarts right now
    if (elapsed > m_duration_ms) m_start_time = now;

    // Tell the caller that the timer expired
    return true;
}
//=========================================================================================================





//=========================================================================================================
// is_expired() - Returns true if our timer was running and has expired
//=========================================================================================================
bool OneShot::is_expired()
{
    // A timer that isn't running is by definition not expired
    if (!m_is_running) return false;

    // Fetch the current timestamp
    unsigned long now = millis();

    // How many milliseconds have elapsed since the timer was started?
    unsigned long elapsed = now - m_start_time;

    // Is the timer expired?
    bool expired = elapsed > m_duration_ms;

    // If it's expired, stop the timer
    if (expired) m_is_running = false;

    // Tell the caller whether or not this timer is expired
    return expired;
}
//=========================================================================================================

