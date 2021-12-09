#include "globals.h"

void CSystem::init()
{
    manual_index = 0;
    setpoint = DEFAULT_SETPOINT;

    // for now
    run_mode = MANUAL;
}

//=========================================================================================================
// reboot() - soft reboots the system
//=========================================================================================================
void CSystem::reboot()
{
    // reboot system
    asm volatile ("jmp 0");
}
//=========================================================================================================


//=========================================================================================================
// return_to_run_mode()
//=========================================================================================================
void CSystem::return_to_run_mode()
{
    if (run_mode == MANUAL)
        ManualModeMgr.start();
    else
        SetpointModeMgr.start();
}
//=========================================================================================================