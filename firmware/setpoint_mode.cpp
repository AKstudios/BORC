#include "globals.h"


//=========================================================================================================
// start() - start setpoint mode
//=========================================================================================================
void CSetpointModeMgr::start()
{   
    // set the system mode and interface mode to setpoint mode
    ee.run_mode = System.iface_mode = SETPOINT_MODE;

    // Tell the temperature controller that the motor may have moved without its permission
    TempCtrl.reset();

    // display the current setpoint
    Display.display(ee.setpoint_f);
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
            if (ee.setpoint_f > MIN_SETPOINT)
            {
                --ee.setpoint_f;
                Display.display(ee.setpoint_f);
                Led.set(BLUE, 1000, true);
            }
            break;

        case KNOB_RIGHT:
            if (ee.setpoint_f < MAX_SETPOINT)
            {
                ++ee.setpoint_f;
                Display.display(ee.setpoint_f);
                Led.set(RED, 1000, true);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
