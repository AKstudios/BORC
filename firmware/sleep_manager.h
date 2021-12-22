#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include "mstimer.h"

//=========================================================================================================
// CSleepMgr - class to manage all sleep related activities
//=========================================================================================================
class CSleepMgr
{
public:

    // checks timer and goes to sleep
    void    execute();

    // kick sleep timer
    void    kick_sleep_timer();

    // this gets called anytime there is activity on the knob
    void    on_knob_activity();

    // Call this to set up the system to automatically go to sleep a few seconds from now
    void    start_sleep_mode();

    // Call this to simulate sleep mode, but the system never actually goes to sleep
    void    start_awake_mode();

    // if anything besides manual mode moves the motor, they have to call this routine
    void    marked_motor_as_moved() {m_last_driven_index = 0xFF;}

protected:

    enum {SLEEP_MODE, AWAKE_MODE} m_mode;

    // Called when the system wakes up due to knob activity
    void    on_wakeup_from_knob();

    // Called when the system wakes up from sleep due to the timer
    void    on_wakeup_from_timer();

    // starts sleep timer 
    void    start_sleep_timer(int timeout_ms=5000);

    // execute() calls one of these
    void    execute_awake_mode();
    void    execute_sleep_mode();

    // This timer expires when it's time to sleep
    OneShot         m_sleep_timer;

    // This timer is used to move the servo every 32 seconds when we're in AWAKE_MODE
    msTimer         m_ctrl_timer;

    // This is the last manual_index we know we drove the motor to
    uint8_t         m_last_driven_index;

    // flag that lets us know if we woke up from knob
    volatile bool m_wakeup_from_knob;
};
//=========================================================================================================

#endif
