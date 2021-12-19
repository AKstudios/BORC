#include "globals.h"

CRotaryKnob         Knob;
CLedMatrix          Display;
CPowerManager       PowerMgr;
CServoDriver        Servo;
CSystem             System;
CManualModeMgr      ManualModeMgr;
CSleepMgr           SleepMgr;
CLedMgr             Led;
CMenuMgr            MenuMgr;
CSetpointModeMgr    SetpointModeMgr;
CTempHum            TempHum;
CSerialServer       SerialServer;
CPIDController      PID;
CUSBSensor          USB;
CEEPROM             EEPROM;
CEEPROM::data_t& ee = EEPROM.data;


int c_to_f(float c) {return (c * 1.8) + 32;}

float f_to_c(int f) {return ((f - 32) * 0.55556);}