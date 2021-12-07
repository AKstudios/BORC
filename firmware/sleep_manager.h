#ifndef _SLEEP_MANAGER_H_
#define _SLEEP_MANAGER_H_

#include "oneshot.h"

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

protected:

    // This timer expires when it's time to sleep
    OneShot         m_sleep_timer;

    // flag that lets us know if we woke up from knob
    bool m_wakeup_from_knob;
};
//=========================================================================================================

#endif