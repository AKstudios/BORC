#include "globals.h"

void CSystem::init()
{
    manual_index = 0;

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


void CSystem::return_to_run_mode()
{
    if (run_mode == MANUAL)
        ManualModeMgr.start();
    else
        //AutoModeMgr.start();
        return;
}
