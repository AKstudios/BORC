#include "globals.h"
#include "display.h"
#include "common.h"

#include <Adafruit_INA219.h>          //https://github.com/adafruit/Adafruit_INA219
#include <Adafruit_PWMServoDriver.h>  //https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

Adafruit_INA219 ina219(CURRENT_SENSE_ADDRESS);
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_DRIVER_ADDRESS);
int value = 10;

//=========================================================================================================

void test()
{
  ina219.begin();
  
  float busVoltage = ina219.getBusVoltage_V();
  float current = ina219.getCurrent_mA();
  Serial.print("initial voltage: ");
  Serial.println(busVoltage);
  Serial.print("initial current: ");
  Serial.println(current);

  pwm.begin();
  pwm.setPWMFreq(50);
  pwm.setPWM(0, 0, 150); // start from min position for 270 servo
  delay(500);
  
  busVoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();
  Serial.print("90 voltage: ");
  Serial.println(busVoltage);
  Serial.print("90 current: ");
  Serial.println(current);

  delay(2500);
  
  pwm.setPWM(0, 0, 500); // start from min position for 270 servo
  delay(100);
  
  busVoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();
  Serial.print("550 voltage: ");
  Serial.println(busVoltage);
  Serial.print("550 current: ");
  Serial.println(current);

  delay(2500);
}

//=========================================================================================================

void setup()
{    
  Serial.begin(115200);
  Serial.println("begin");
  
  PowerMgr.init();
  PowerMgr.powerOn(CURRENT_SENSE_POWER_PIN);
  PowerMgr.powerOn(LED_SCREEN_POWER_PIN);
  PowerMgr.powerOn(TEMP_SENSOR_POWER_PIN);
  PowerMgr.powerOn(DRIVER_POWER_PIN);
  PowerMgr.powerOn(SERVO_POWER_PIN);

  test();
  
  Knob.init();
  Display.init();
}

//=========================================================================================================

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
