#include "globals.h"

CRotaryKnob Knob;
CLedMatrix Display;
CPowerManager PowerMgr;
CServoDriver Servo;
CSystem System;
CManualModeMgr ManualModeMgr;
CSleepMgr SleepMgr;
CLedMgr Led;
CMenuMgr MenuMgr;
CSetpointModeMgr SetpointModeMgr;
CEEPROM EEPROM;

CEEPROM::data_t& ee = EEPROM.data;