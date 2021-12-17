//=========================================================================================================
// serialserver.cpp - Handler functions for our serial server
//=========================================================================================================
#include "serialserver.h"
#include <string.h>
#include "globals.h"
#include "strfloat.h"

// Compares a token to a string constant.  The string constant can be in RAM or Flash
#define token_is(strcon) ((compare_token(token,strcon)))

// Non AVR systems will need this definition
#ifndef __AVR__
#define PROGMEM
#endif

//=========================================================================================================
// on_command() - The top level dispatcher for commands
// 
// Passed:  token = Pointer to the command string
//=========================================================================================================
void CSerialServer::on_command(const char* token)
{

    if      token_is("fwrev")   handle_fwrev();
    else if token_is("nv")      handle_nv();
    else if token_is("ee")      handle_nv();
    else if token_is("reboot")  handle_reboot();
    else if token_is("help")    handle_help();
    else if token_is("nvset")   handle_nvset();
    else if token_is("eeset")   handle_nvset();

    else fail_syntax();
}
//=========================================================================================================


//=========================================================================================================
// handle_fwrev() - Reports the firmware revision
//=========================================================================================================
bool CSerialServer::handle_fwrev()
{
    return pass(FW_VERSION);
}
//=========================================================================================================


//=========================================================================================================
// handle_nv() - Handles these commands:
//                    nv
//                    nv dirty
//                    nv destroy
// 
//=========================================================================================================
bool CSerialServer::handle_nv()
{
    const char* token;


    // Fetch the next token.  If we can't, the user wants us to dump EEPROM
    if (!get_next_token(&token))
    {
        show_nv(&EEPROM.clean);
        return pass();
    }

    // Does the user want to see the clean version?
    if token_is("dirty")
    {
        show_nv(&EEPROM.data);
        return pass();
    }

    // Does the user want to destroy the EEPROM?
    if token_is("destroy")
    {
        EEPROM.destroy();
        return pass();
    }

    // If we get here, there was a syntax error
    return fail_syntax();
}
//=========================================================================================================


//=========================================================================================================
// show_nv() - Displays either the clean or dirty EEPROM structure.
// 
// Passed:  ee = a pointer to either the clean or dirty structure
//=========================================================================================================
void CSerialServer::show_nv(void* vp)
{
    const char* space = " ";

    const char run_mode_m[]   PROGMEM = "run_mode           : 0 - MANUAL";
    const char run_mode_s[]   PROGMEM = "run_mode           : 2 - SETPOINT";
    const char run_mode[]     PROGMEM = "run_mode           : %i";
    const char manual_index[] PROGMEM = "manual_index       : %i";
    const char setpoint[]     PROGMEM = "setpoint           : %i";
    const char orientation[]  PROGMEM = "orientation        : %i";
    const char is_servocal[]  PROGMEM = "is_servo_cal       : %i";
    const char kp[]           PROGMEM = "kp                 : %s";
    const char ki[]           PROGMEM = "ki                 : %s";
    const char kd[]           PROGMEM = "kd                 : %s";


    // Get a handy reference to the EEPROM structure the caller wants to dump
    CEEPROM::data_t& ee = *(CEEPROM::data_t*)vp;

    // Display the run mode, decoded into plain English
    switch (ee.run_mode)
    {
        case MANUAL:
            replyf(run_mode_m);
            break;
        case SETPOINT:
            replyf(run_mode_s);
            break;
        default:
            replyf(run_mode, ee.run_mode);
    }

    // Now display all of the other EEPROM settings
    replyf(manual_index,    ee.manual_index);
    replyf(setpoint,        ee.setpoint);
    replyf(orientation,     ee.orientation);
    replyf(is_servocal,     ee.is_servo_calibrated);
    replyf(kp,              strfloat(ee.kp, 0, 3));
    replyf(ki,              strfloat(ee.ki, 0, 3));
    replyf(kd,              strfloat(ee.kd, 0, 3));
}
//=========================================================================================================


//=========================================================================================================
// handle_reboot() - Sends the response, then reboot the device
//=========================================================================================================
bool CSerialServer::handle_reboot()
{
    // Send out the response so the client knows this worked
    pass();

    // Wait a half-second for the response to finish transmitting
    delay(500);

    // And reboot the system
    System.reboot();

    // The return value here is meaningless
    return true;
}
//=========================================================================================================




//=========================================================================================================
// handle_help() - Displays help text
//=========================================================================================================
bool CSerialServer::handle_help()
{
    
    const char line_01  [] PROGMEM = "ee                - Displays EEPROM contents";
    const char line_02  [] PROGMEM = "ee dirty          - Displays EEPROM shadow RAM";
    const char line_03  [] PROGMEM = "ee destroy        - Erases EEPROM";
    const char line_04  [] PROGMEM = "fwrev             - Displays firmware revision";
    const char line_05  [] PROGMEM = "reboot            - Soft reboots device";
    const char line_06  [] PROGMEM = "eeset kp <value>  - Saves PID P constant to EEPROM";
    const char line_07  [] PROGMEM = "eeset ki <value>  - Saves PID I constant to EEPROM";
    const char line_08  [] PROGMEM = "eeset kd <value>  - Saves PID D constant to EEPROM";


    replyf(line_01);
    replyf(line_02);
    replyf(line_03);
    replyf(line_04);
    replyf(line_05);
    replyf(line_06);
    replyf(line_07);
    replyf(line_08);

    return pass();
}
//=========================================================================================================


//=========================================================================================================
// handle_nvset() - Handles these commands:
//                    nvset kp <value>
//                    nvset ki <value>
//                    nvset kd <value>
//=========================================================================================================
bool CSerialServer::handle_nvset()
{
    const char *token, *name, *value;

    // Fetch the next token, it should be a field name
    if (!get_next_token(&name)) return fail_syntax();

    // Fetch the next token, it should be a field value
    if (!get_next_token(&value)) return fail_syntax();
    
    // Get a floating point version of the value
    float fvalue = (float)atof(value);

    // We want to examine the name token
    token = name;

    // Handle "nvset kp"
    if token_is("kp")
    {
        ee.kp = fvalue;
        EEPROM.write();
        return pass();
    }

    // Handle "nvset ki"
    if token_is("ki")
    {
        ee.ki = fvalue;
        EEPROM.write();
        return pass();
    }

    // Handle "nvset kd"
    if token_is("kd")
    {
        ee.kd = fvalue;
        EEPROM.write();
        return pass();
    }

    // If we get here, there was a syntax error
    return fail_syntax();
}
//=========================================================================================================

