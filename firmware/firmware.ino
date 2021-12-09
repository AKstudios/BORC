#include "globals.h"
#include "common.h"
#include <avr/wdt.h>

//=========================================================================================================
// Setup
//=========================================================================================================
void setup()
{    
  Serial.begin(115200);
  Serial.println("begin");
  
  System.init();

  PowerMgr.init();
  PowerMgr.powerOn(CURRENT_SENSE_POWER_PIN);
  PowerMgr.powerOn(LED_SCREEN_POWER_PIN);
  PowerMgr.powerOn(TEMP_SENSOR_POWER_PIN);
  PowerMgr.powerOn(DRIVER_POWER_PIN);
  PowerMgr.powerOn(SERVO_POWER_PIN);
  
  Knob.init(CHANNEL_A, CHANNEL_B, CLICK_PIN);
  Display.init();
  Servo.init();
  Led.init();

  ManualModeMgr.start();
  SleepMgr.init();
  
  // Servo.calibrate_bare();

  // Servo.move_to_pwm(92, 4000, true);
  // Servo.move_to_pwm(542, 4000, true);  
}
//=========================================================================================================

//=========================================================================================================
// BIG LOOP
//=========================================================================================================
void loop()
{
  wdt_reset(); //pat the dog...

  SleepMgr.execute();

  Led.execute();

  switch (System.iface_mode)
  {
    case MANUAL   : ManualModeMgr.execute();    break;
    case MENU     : MenuMgr.execute();          break;
    case SETPOINT : SetpointModeMgr.execute();  break;
  }
}
//=========================================================================================================
