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
// 1002  22-Dec-21  DWW      Made entire system use Farenheit instead of a mix of F and C.
//                           Fixed minor timer bug in Menu Manager
//                           Replaced "TempHum" with our own SHT31 interface class
//                           Replaced Adafruit INA219 driver with our own
//                           Added "clog" command for current logging
//                           Added "servo" command to move the servo to a position
//                           Replaced all "Wire.XXX()" calls with the twi_XXX() API
//                           Now setting a system-wide I2C bus-speed of 400 Khz
//                           Added command eeset encrypt "16_byte_encr_key"
//                           Added command eeset network <network_id>
//                           Added command eeset node <node_id>
//                           Added command "transmit"
//                           On wakeup, we now transmit a telemetry packet to the gateway
//
// 1003  07-Jan-22  ASA      Added temp, hum, manual index in radio telemetry struct (now rearranged)
//                           Humidity now stored in System variable called "hum"
//                           Firmware now reads unique 64bit ID from SPI Flash for testing
//
// 1004  21-Feb-22  ASA      Added error byte, transaction ID and firmware version to telemetry and config packets
//                           Error byte now checks, sets and clear bits based on various systems
//                           Pinmodes before sleep ensures lowest sleep current (~20uA)
//                           Telemetry packet now expects a response
//                           Added config mode - sends config packets and waits for response
//
// 1005  25-Feb-22  ASA      Reinitialize display after waking up from sleep so it doens't wonk out
//                           
//=========================================================================================================
#define FW_VERSION 1005

#endif