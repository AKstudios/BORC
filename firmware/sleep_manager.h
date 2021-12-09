#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include "mstimer.h"

//=========================================================================================================
// CSleepMgr - class to manage all sleep related activities
//=========================================================================================================
class CSleepMgr
{
public:
    // setup sleep mode
    void init();

    // checks timer and goes to sleep
    void execute();

    // starts sleep timer 
    void start_timer(int timeout_ms=5000);

    // this gets called anytime there is activity on the knob
    void on_knob_activity();

protected:
    
    void wakeup_from_knob();

    void wakeup_from_timer();


    // This timer expires when it's time to sleep
    OneShot         m_sleep_timer;

    // flag that lets us know if we woke up from knob
    volatile bool m_wakeup_from_knob;
};
//=========================================================================================================

#endif
