
//=========================================================================================================
// mstimer.h - Defines a rollover-safe one-shot timer and a rollover-safe repeating timer
//=========================================================================================================
#ifndef _MSTIMER_H_
#define _MSTIMER_H_

//--------------------------------------------------------------------------------------------------------
// This is a rollover-safe repeating timer, and the base-class for a one-shot timer
//--------------------------------------------------------------------------------------------------------
class msTimer
{
public:

    // Constructor : timer is instantiated in the "stopped" condition
    msTimer() { m_is_running = false; }

    // Call this to start or restart the timer
    void    start(unsigned int duration_ms);

    // call this to stop the timer
    void    stop() {m_is_running = false;}

    // Call this to find out if the timer is running and expired.
    bool    is_expired();

    // check if the timer is still running
    bool    is_running() {return m_is_running;}

protected:

    // The is the timerstamp when the timer was started
    unsigned long m_start_time;

    // This is the duration of the one-shot timer in milliseconds
    unsigned int  m_duration_ms;

    // This is true if the timer is running 
    bool          m_is_running;
};


//--------------------------------------------------------------------------------------------------------
// This is a rollover-safe OneShot timer
//--------------------------------------------------------------------------------------------------------
class OneShot : public msTimer
{
public:

    // Call this to find out if the timer is running and expired.
    // If this returns 'true', the timer has been stopped
    bool    is_expired();
};


#endif