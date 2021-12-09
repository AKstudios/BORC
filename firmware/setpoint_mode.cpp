#include "globals.h"

//=========================================================================================================
// start() - start setpoint mode
//=========================================================================================================
void CSetpointModeMgr::start()
{
    System.run_mode = System.iface_mode = SETPOINT;

    // display the current setpoint
    Display.display(System.setpoint);
}
//=========================================================================================================


//=========================================================================================================
// execute() - setpoint mode state machine
//=========================================================================================================
void CSetpointModeMgr::execute()
{
    knob_event_t event;

    while (Knob.get_event(&event))
    {
        
        switch (event)
        {
        case KNOB_LEFT:
            if (System.setpoint > MIN_SETPOINT)
            {
                --System.setpoint;
                Display.display(System.setpoint);
            }
            break;

        case KNOB_RIGHT:
            if (System.setpoint < MAX_SETPOINT)
            {
                ++System.setpoint;
                Display.display(System.setpoint);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
