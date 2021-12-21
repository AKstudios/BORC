//=========================================================================================================
// serialserver.cpp - Handler functions for our serial server
//=========================================================================================================
#include "serialserver.h"
#include <string.h>
#include "globals.h"


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

    if      token_is("fwrev")    handle_fwrev();
    else if token_is("nv")       handle_nv();
    else if token_is("ee")       handle_nv();
    else if token_is("reboot")   handle_reboot();
    else if token_is("help")     handle_help();
    else if token_is("nvset")    handle_nvset();
    else if token_is("eeset")    handle_nvset();
    else if token_is("sim")      handle_sim();
    else if token_is("temp")     handle_temp();
    else if token_is("setpoint") handle_setpoint();
    else if token_is("ui")       handle_ui();

    else fail_syntax();
}
//=========================================================================================================


//=========================================================================================================
// handle_ui() - Simulates user interactions
//=========================================================================================================
bool CSerialServer::handle_ui()
{
    const char* token;

    // Fetch the next token, it will tell us what user interaction to simulate
    if (!get_next_token(&token)) return fail_syntax();

    if (compare_token(token, "l") || compare_token(token, "left"))
    {
        Knob.add_event(KNOB_LEFT);
        return pass();
    }

    if (compare_token(token, "r") || compare_token(token, "right"))
    {
        Knob.add_event(KNOB_RIGHT);
        return pass();
    }

    if (compare_token(token, "c") || compare_token(token, "click"))
    {
        Knob.add_event(KNOB_UP);
        return pass();
    }

    if (compare_token(token, "lp") || compare_token(token, "lpress"))
    {
        Knob.add_event(KNOB_LPRESS);
        return pass();
    }

    // If we get here, we don't recognize what the user wants us to do
    return fail_syntax();
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
// handle_temp() - Report the temperature in both C and F
//=========================================================================================================
bool CSerialServer::handle_temp()
{
    // Fetch the current temperature in C
    float temp_c = TempHum.read_temp_c();

    // Fetch the Farenheit version of the temperature
    int temp_f = c_to_f(temp_c);

    // Report them both to the user
    return pass("%s %i (temp)", strfloat(temp_c, 0, 2), temp_f);
}
//=========================================================================================================



//=========================================================================================================
// handle_setpoint() - Sets the temp controller setpoint
//=========================================================================================================
bool CSerialServer::handle_setpoint()
{
    const char* token;

    // Fetch the temperature we want to use as a setpoint
    if (!get_next_token(&token)) return fail_syntax();

    // Fetch the current temperature in C
    float temp_c = atof(token);
    
    // Fetch the Farenheit version of the temperature
    int temp_f = c_to_f(temp_c);

    // Store the new setpoint in EEPROM
    ee.setpoint_f = temp_f;

    // Tell the temperature controller what to use for a setpoint
    TempCtrl.new_setpoint_f(temp_f);

    // Put the system in setpoint mode
    SetpointModeMgr.start();

    // Report them both to the user
    return pass("%s %i (setpoint)", strfloat(temp_c, 0, 2), temp_f);
}
//=========================================================================================================





//=========================================================================================================
// handle_sim() - Handles the following commands:
//                   sim temp <value>
//=========================================================================================================
bool CSerialServer::handle_sim()
{
    const char* token;

    // Find out what the user wants to simulate
    if (!get_next_token(&token)) return fail_syntax();

    // If the user wants to simulate a room temperature
    if token_is("temp")
    {
        // Fetch the temperature they want to use
        if (!get_next_token(&token)) return fail_syntax();

        // Convert the token to float, in degrees C
        float temp_c = atof(token);

        // "sim temp off" means stop simulating
        if token_is("off") temp_c = -100.0;

        // Tell TempHum to simulate this room temp.  99 degrees = "stop simulating"
        TempHum.simulate_temp_c(temp_c);

        // Tell the user that all is well.
        return pass("%s %i (sim_temp)", strfloat(temp_c, 0, 2), c_to_f(temp_c));
    }

    // If we get here, we have no idea what the user is talking about
    return fail_syntax();
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
    const char servo_min[]    PROGMEM = "servo_min          : %i";
    const char servo_max[]    PROGMEM = "servo_max          : %i";
    const char notches[]      PROGMEM = "notches            : %i";
    const char tcm[]          PROGMEM = "tcm                : %i";
    const char deadband[]     PROGMEM = "deadband           : %s";



    // Get a handy reference to the EEPROM structure the caller wants to dump
    CEEPROM::data_t& ee = *(CEEPROM::data_t*)vp;

    // Display the run mode, decoded into plain English
    switch (ee.run_mode)
    {
        case MANUAL_MODE:
            replyf(run_mode_m);
            break;
        case SETPOINT_MODE:
            replyf(run_mode_s);
            break;
        default:
            replyf(run_mode, ee.run_mode);
    }

    // Now display all of the other EEPROM settings
    replyf(manual_index,    ee.manual_index);
    replyf(setpoint,        ee.setpoint_f);
    replyf(orientation,     ee.orientation);
    replyf(is_servocal,     ee.is_servo_calibrated);
    replyf(servo_min,       ee.servo_min);
    replyf(servo_max,       ee.servo_max);
    replyf(kp,              strfloat(ee.kp, 0, 3));
    replyf(ki,              strfloat(ee.ki, 0, 3));
    replyf(kd,              strfloat(ee.kd, 0, 3));
    replyf(notches,         ee.notches);
    replyf(tcm,             ee.tcm);
    replyf(deadband,        strfloat(ee.deadband, 0, 2));

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
    
    const char line_01  [] PROGMEM = "ee                          - Displays EEPROM contents";
    const char line_02  [] PROGMEM = "ee dirty                    - Displays EEPROM shadow RAM";
    const char line_03  [] PROGMEM = "ee destroy                  - Erases EEPROM";
    const char line_04  [] PROGMEM = "fwrev                       - Displays firmware revision";
    const char line_05  [] PROGMEM = "reboot                      - Soft reboots device";
    const char line_06  [] PROGMEM = "eeset kp <value>            - Saves PID P constant to EEPROM";
    const char line_07  [] PROGMEM = "eeset ki <value>            - Saves PID I constant to EEPROM";
    const char line_08  [] PROGMEM = "eeset kd <value>            - Saves PID D constant to EEPROM";
    const char line_09  [] PROGMEM = "eeset is_servocal <value>   - Saves servo calibration flag";
    const char line_10  [] PROGMEM = "eeset notches <value>       - Save new notch count for temp ctrl";
    const char line_11  [] PROGMEM = "eeset tcm <value>           - Save new time-constant multiplier";
    const char line_12  [] PROGMEM = "eeset deadband <value>      - Save new temperature deadband size";
    const char line_13  [] PROGMEM = "sim temp <deg_C>            - Simulates the room temperature";
    const char line_14  [] PROGMEM = "temp                        - Reports the room temperature";
    const char line_15  [] PROGMEM = "setpoint <deg_c>            - Sets new temp control setpoint";
    const char line_16  [] PROGMEM = "ui <l | left>               - Simulate knob rotate left";
    const char line_17  [] PROGMEM = "ui <r | right>              - Simulate knob rotate right";
    const char line_18  [] PROGMEM = "ui <c | click>              - Simulate knob click";
    const char line_19  [] PROGMEM = "ui <lp | lpress>            - Simulate knob long press";
        

    replyf(line_01);
    replyf(line_02);
    replyf(line_03);
    replyf(line_04);
    replyf(line_05);
    replyf(line_06);
    replyf(line_07);
    replyf(line_08);
    replyf(line_09);
    replyf(line_10);
    replyf(line_11);
    replyf(line_12);
    replyf(line_13);
    replyf(line_14);
    replyf(line_15);
    replyf(line_16);
    replyf(line_17);
    replyf(line_18);
    replyf(line_19);

    return pass();
}
//=========================================================================================================


//=========================================================================================================
// handle_nvset() - Handles these commands:
//                    nvset kp <value>
//                    nvset ki <value>
//                    nvset kd <value>
//                    nvset notches <value>
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
        PID.set_constants(ee.kp, ee.ki, ee.kd);
        return pass();
    }

    // Handle "nvset ki"
    if token_is("ki")
    {
        ee.ki = fvalue;
        EEPROM.write();
        PID.set_constants(ee.kp, ee.ki, ee.kd);
        return pass();
    }

    // Handle "nvset kd"
    if token_is("kd")
    {
        ee.kd = fvalue;
        EEPROM.write();
        PID.set_constants(ee.kp, ee.ki, ee.kd);
        return pass();
    }

    if token_is("is_servocal")
    {
        ee.is_servo_calibrated = int(fvalue);
        EEPROM.write();
        return pass();
    }

    if token_is("notches")
    {
        ee.notches = int(fvalue);
        EEPROM.write();
        return pass();
    }

    if token_is("tcm")
    {
        ee.tcm = int(fvalue);
        EEPROM.write();
        return pass();
    }

    if token_is("deadband")
    {
        ee.deadband = fvalue;
        EEPROM.write();
        return pass();
    }


    // If we get here, there was a syntax error
    return fail_syntax();
}
//=========================================================================================================

