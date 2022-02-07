#include "globals.h"

CRotaryKnob         Knob;
IS31FL3731          Display;
CPowerManager       PowerMgr;
CServoDriver        Servo;
CSystem             System;
CManualModeMgr      ManualModeMgr;
CSleepMgr           SleepMgr;
CLedMgr             Led;
CMenuMgr            MenuMgr;
CSetpointModeMgr    SetpointModeMgr;
CSHT31              SHT31(TEMP_SENSE_ADDRESS);
CINA219             INA219;
CSerialServer       SerialServer;
CUSBSensor          USB;
CEEPROM             EEPROM;
CNotchController    TempCtrl;
CSerialLogger       Logger;
CCurrentLogger      CurLogger;
CBattSensor         Battery;
CRadio              Radio;
SPIFlash            Flash(FLASH_SS, 0xEF30); //EF30 for 4mbit  Windbond chip (W25X40CL);

// pointer to EEPROM dirty data buffer
CEEPROM::data_t& ee = EEPROM.data;


int c_to_f(float c) {return (c * 1.8) + 32;}
float f_to_c(int f) {return ((f - 32) * 0.55556);}