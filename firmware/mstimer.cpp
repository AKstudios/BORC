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

    // If the timer has been kicked, restart it
    if (m_is_kicked)
    {
        m_is_kicked = false;
        m_start_time = now;
        return false;
    }

    //------------------------------------------------------------------------------------
    // If we get here, the timer is running.  Now we need to determine whether the timer
    // has exceeded its duration and should therefore stop running. (i.e., whether it has
    // expired)
    //------------------------------------------------------------------------------------

    // How many milliseconds have elapsed since the timer was started?
    unsigned long elapsed = now - m_start_time;

    // Should the timer continue running?
    m_is_running = elapsed <= m_duration_ms;

    // Tell the caller whether or not this timer is expired
    return !m_is_running;
}
//=========================================================================================================


//=========================================================================================================
// start_from_isr() - Starts the timer in a thread-safe manner
//=========================================================================================================
void ThreadSafeOneShot::start_from_isr()
{
    m_start_time_from_isr = millis();
    m_is_started_from_isr = true;
}
//=========================================================================================================


//=========================================================================================================
// is_expired() - Returns true if our timer was running and has expired
//=========================================================================================================
bool ThreadSafeOneShot::is_expired()
{
    // If this timer was started (or restarted) from an ISR...
    if (m_is_started_from_isr)
    {
        // Clear the flag.
        m_is_started_from_isr = false;

        // Find out when the timer was started, in a thread-safe manner
        cli();
        m_start_time = m_start_time_from_isr;
        sei();

        // The timer is now running
        m_is_running = true;
    }

    // A timer that isn't running is by definition not expired
    if (!m_is_running) return false;

    // Fetch the current timestamp
    unsigned long now = millis();

    // If the timer has been kicked, restart it
    if (m_is_kicked)
    {
        m_is_kicked = false;
        m_start_time = now;
        return false;
    }

    //------------------------------------------------------------------------------------
    // If we get here, the timer is running.  Now we need to determine whether the timer
    // has exceeded its duration and should therefore stop running. (i.e., whether it has
    // expired)
    //------------------------------------------------------------------------------------

    // How many milliseconds have elapsed since the timer was started?
    unsigned long elapsed = now - m_start_time;

    // Should the timer continue running?
    m_is_running = elapsed <= m_duration_ms;

    // Tell the caller whether or not this timer is expired
    return !m_is_running;
}
//=========================================================================================================

