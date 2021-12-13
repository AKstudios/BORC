#include "globals.h"

//=========================================================================================================
// start() - start manual mode
//=========================================================================================================
void CManualModeMgr::start()
{
    System.iface_mode = ee.run_mode;

    // display the current manual mode index
    Display.display(ee.manual_index);
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
                Display.display(ee.manual_index);
            }
            break;

        case KNOB_RIGHT:
            if (ee.manual_index < Servo.get_max_index())
            {
                ++ee.manual_index;
                Display.display(ee.manual_index);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
