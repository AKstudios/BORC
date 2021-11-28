#include "globals.h"
#include "display.h"

int value = 10;

void setup()
{
  Serial.begin(115200);
  Serial.println("begin");
  Knob.init();
  Display.init();

  for (int i=0;i<2;i++)
  {
  Display.display(11);
  Display.display(22);
  Display.display(33);
  Display.display(44);
  Display.display(55);
  }
}


void loop()
{
  knob_event_t event;

  if (Knob.get_event(&event))
  {
    
    switch(event)
    {
      case KNOB_LEFT:
        Serial.println("knob left\n");
        Display.display(--value);
        break;

      case KNOB_RIGHT:
        Serial.println("knob righttt\n");
        Display.display(++value);
        break;

      case KNOB_CLICK:
        Serial.println("knob click\n"); break;

      default:
        Serial.println("HOW THE FUCK.. "); break;
    }
  }
}
