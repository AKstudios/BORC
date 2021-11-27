#include "globals.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("begin");
  Knob.init();
}


void loop()
{
  knob_event_t event;

  if (Knob.get_event(&event))
  {
    
    switch(event)
    {
      case KNOB_LEFT:
        Serial.println("knob left\n"); break;

      case KNOB_RIGHT:
        Serial.println("knob righttt\n"); break;

      case KNOB_CLICK:
        Serial.println("knob click\n"); break;

      default:
        Serial.println("HOW THE FUCK.. "); break;
    }
  }
}
