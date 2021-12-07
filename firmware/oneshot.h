//=========================================================================================================
// oneshot.h - Defines a rollover-safe one-shot timer
//=========================================================================================================
#ifndef _ONESHOT_H_
#define _ONESHOT_H_

class OneShot
{
public:

    // Constructor : timer is instantiated in the "stopped" condition
    OneShot() {m_is_running = false;}

    // Call this to start or restart the timer
    void    start(unsigned int duration_ms);

    // call this to stop the timer
    void    stop() {m_is_running = false;}

    // Call this to find out if the one-shot timer is running and expired.
    // If this returns 'true', it stops the timer
    bool    is_expired();

    // check if the timer is still running
    bool    is_running() {return m_is_running;}

protected:

    // The is the timerstamp when the timer was started
    unsigned long m_start_time;

    // This is the duration of the one-shot timer in milliseconds
    unsigned int m_duration_ms;

    // This is true if the timer is running 
    bool    m_is_running;
};


#endif
