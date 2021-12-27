//=========================================================================================================
// changelog.h - Defines the update history for this source code
//=========================================================================================================
#ifndef _CHANGELOG_H_
#define _CHANGELOG_H_

//=========================================================================================================
// Vers   When      Who      What
//---------------------------------------------------------------------------------------------------------
// 1000  13-Dec-21  ASA      Initial creation
//
// 1001  22-Dec-21  ASA/DWW  First Release
//
// xxxx  22-Dec-21  DWW      Made entire system use Farenheit instead of a mix of F and C.
//                           Fixed minor timer bug in Menu Manager
//                           Replaced "TempHum" with our own SHT31 interface class
//                           Replaced Adafruit INA219 driver with our own
//                           Added "clog" command for current logging
//                           Added "servo" command to move the servo to a position
//                           Replaced all "Wire.XXX()" calls with the twi_XXX() API
//                           Now setting a system-wide I2C bus-speed of 400 Khz
//=========================================================================================================
#define FW_VERSION "xxxx"

#endif