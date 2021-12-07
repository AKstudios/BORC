#include "globals.h"

void CManualModeMgr::start()
{
    System.mode = MANUAL;
    Display.display(System.manual_index);
}

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
                Servo.move_to_index(System.manual_index);
            }
            break;

        case KNOB_RIGHT:
            if (System.manual_index < Servo.get_max_index())
            {
                ++System.manual_index;
                Display.display(System.manual_index);
                Servo.move_to_index(System.manual_index);
            }
            break;
        }
    }
}