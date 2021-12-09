#include "globals.h"

//=========================================================================================================
// start() - start manual mode
//=========================================================================================================
void CManualModeMgr::start()
{
    System.run_mode = System.iface_mode = MANUAL;

    // display the current manual mode index
    Display.display(System.manual_index);
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
            if (System.manual_index > 0)
            {
                --System.manual_index;
                Display.display(System.manual_index);
            }
            break;

        case KNOB_RIGHT:
            if (System.manual_index < Servo.get_max_index())
            {
                ++System.manual_index;
                Display.display(System.manual_index);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
