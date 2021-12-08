#include "globals.h"

void CManualModeMgr::start()
{
    System.mode = MANUAL;
    Display.display(System.manual_index);
}
//--------------------------------------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------------------------------------
void CManualModeMgr::execute()
{
    static int color = 0;

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
        
        case KNOB_UP:
            ++color;
//            Led.set(led_color_t(color), 250);
//            Serial.println(color & 7);
            break;
        }
    }
}
