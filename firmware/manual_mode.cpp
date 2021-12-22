#include "globals.h"

//=========================================================================================================
// start() - start manual mode
//=========================================================================================================
void CManualModeMgr::start()
{
    ee.run_mode = System.iface_mode = MANUAL_MODE;

    // display the current manual mode index
    Display.print(ee.manual_index);
}
//=========================================================================================================


//=========================================================================================================
// execute() - manual mode execute
//=========================================================================================================
void CManualModeMgr::execute()
{
    knob_event_t event;

    while (Knob.get_event(&event))
    {
        
        switch (event)
        {
        case KNOB_LEFT:
            if (ee.manual_index > 0)
            {
                --ee.manual_index;
                Display.print(ee.manual_index);
                Led.set(BLUE, 1000, true);
            }
            break;

        case KNOB_RIGHT:
            if (ee.manual_index < Servo.get_max_index())
            {
                ++ee.manual_index;
                Display.print(ee.manual_index);
                Led.set(RED, 1000, true);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
