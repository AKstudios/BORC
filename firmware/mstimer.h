
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

    // Constructor
    OneShot() { m_is_kicked = false; }

    // Call this to find out if the timer is running and expired.
    // If this returns 'true', the timer has been stopped
    bool    is_expired();

    // Restarts the timer in a thread-safe manner
    void    kick() { m_is_kicked = true; }

protected:

    // When this is true, is_expired() will restart the timer
    bool    m_is_kicked;

};
//--------------------------------------------------------------------------------------------------------



//--------------------------------------------------------------------------------------------------------
// This is a one-shot timer that includes the ability to start it from an ISR in a thread-safe manner
//--------------------------------------------------------------------------------------------------------
class ThreadSafeOneShot : public OneShot
{
public:

    // Call this to set the durtation of the timer that will be started via "start_from_isr()"
    void    set_duration(unsigned int duration_ms) { m_duration_ms = duration_ms; }

    // Starts the timer from an ISR in a thread-safe manner.  Duration must have been previously set
    void    start_from_isr();

    // Call this to find out if the timer is running and expired.
    bool    is_expired();


protected:

    // This will be true when the timer has been started from an ISR
    bool          m_is_started_from_isr;

    // This is the timestamp when the ISR called "start_from_isr"
    unsigned long m_start_time_from_isr;

};
//--------------------------------------------------------------------------------------------------------


#endif