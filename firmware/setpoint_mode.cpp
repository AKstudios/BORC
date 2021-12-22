#include "globals.h"


//=========================================================================================================
// start() - start setpoint mode
//=========================================================================================================
void CSetpointModeMgr::start()
{   
    // set the system mode and interface mode to setpoint mode
    ee.run_mode = System.iface_mode = SETPOINT_MODE;

    // display the current setpoint
    Display.print(ee.setpoint_f);
}
//=========================================================================================================


//=========================================================================================================
// execute() - setpoint mode state machine
//=========================================================================================================
void CSetpointModeMgr::execute()
{
    knob_event_t event;
    static OneShot vbar_timer;

    while (Knob.get_event(&event))
    {
        switch (event)
        {
        case KNOB_LEFT:
            if (ee.setpoint_f > MIN_SETPOINT)
            {
                --ee.setpoint_f;
                Display.print(ee.setpoint_f);
                Led.set(BLUE, 1000, true);
            }
            else
            {
                Display.vertical_bar(0, true);
                vbar_timer.start(500);                
            }
            break;

        case KNOB_RIGHT:
            if (ee.setpoint_f < MAX_SETPOINT)
            {
                ++ee.setpoint_f;
                Display.print(ee.setpoint_f);
                Led.set(RED, 1000, true);
            }
            else
            {
                Display.vertical_bar(14, true);
                vbar_timer.start(500);                
            }
            break;

        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }

    }

    // If the vertical bar timer is expired, redisplay the text to remove the bar
    if (vbar_timer.is_expired()) Display.print(ee.setpoint_f);
}
//=========================================================================================================
