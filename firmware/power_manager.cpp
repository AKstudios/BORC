//=========================================================================================================
// power_manager.cpp - turn devices on and off
//=========================================================================================================
#include "power_manager.h"
#include "Arduino.h"
#include "common.h"

// array of all power pins
static char pin[] =
{
  DRIVER_POWER_PIN,
  LED_SCREEN_POWER_PIN,
  TEMP_SENSOR_POWER_PIN,
  CURRENT_SENSE_POWER_PIN,
  SERVO_POWER_PIN
};

//=========================================================================================================
// init() - set all power pins to outputs once
//=========================================================================================================
void CPowerManager::init()
{
  for (int i=0; i<sizeof(pin); i++)
    pinMode(pin[i], OUTPUT);
}

//=========================================================================================================

//=========================================================================================================
// powerOn() - turns on a specific device
//=========================================================================================================
void CPowerManager::powerOn(int pin_number)
{
  digitalWrite(pin_number, HIGH);
}

//=========================================================================================================

//=========================================================================================================
// powerOff() - turns off a specific device
//=========================================================================================================
void CPowerManager::powerOff(int pin_number)
{
  digitalWrite(pin_number, LOW);
}

//=========================================================================================================

void CPowerManager::powerI2C()
{
  digitalWrite(SERVO_POWER_PIN, HIGH);
  digitalWrite(DRIVER_POWER_PIN, HIGH);
  digitalWrite(LED_SCREEN_POWER_PIN, HIGH);
  digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);
  digitalWrite(CURRENT_SENSE_POWER_PIN, HIGH);
}
