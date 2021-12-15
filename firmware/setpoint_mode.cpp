#include "globals.h"

//=========================================================================================================
// start() - start setpoint mode
//=========================================================================================================
void CSetpointModeMgr::start()
{   
    ee.run_mode = System.iface_mode = SETPOINT;

    // display the current setpoint
    Display.display(ee.setpoint);
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
            if (ee.setpoint > MIN_SETPOINT)
            {
                --ee.setpoint;
                Display.display(ee.setpoint);
                Led.set(BLUE, 1000, true);
            }
            break;

        case KNOB_RIGHT:
            if (ee.setpoint < MAX_SETPOINT)
            {
                ++ee.setpoint;
                Display.display(ee.setpoint);
                Led.set(RED, 1000, true);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
