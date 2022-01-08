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
    else if token_is("clog")     handle_clog();
    else if token_is("servo")    handle_servo();
    else if token_is("transmit") handle_transmit();

    else fail_syntax();
}
//=========================================================================================================


//=========================================================================================================
// handle_servo() - Drive the servo to the specified position
//=========================================================================================================
bool CSerialServer::handle_servo()
{
    const char* token;

    // Fetch the next token, it will be a servo position
    if (!get_next_token(&token)) return fail_syntax();

    // Fetch the PWM value that specifies our desired servo position
    int pwm = atoi(token);

    // Tell the servo to move to that position
    Servo.move_to_pwm(pwm, 5000);

    // Tell the client that all is well
    return pass();
}
//=========================================================================================================




//=========================================================================================================
// handle_clog() - Turn current logging on or off
//=========================================================================================================
bool CSerialServer::handle_clog()
{
    const char* token;
    const char* ascii_seconds;

    // Fetch the next token, it will tell whether to start or stop logging
    if (!get_next_token(&token)) return fail_syntax();

    // And fetch the token that tells how how many seconds to start for.  (it may not exist)
    get_next_token(&ascii_seconds);

    // Convert the number of seconds from ASCII to binary
    int seconds = atoi(ascii_seconds);

    // Are we starting the logger?
    if token_is("start")
    {
        CurLogger.start(seconds);
        return pass();
    }

    // Are we stopping the logger
    if token_is("stop")
    {
        CurLogger.stop();
        return pass();
    }

    // If we get here, we have no idea what the user is asking for
    return fail_syntax();
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
    float temp_f = 0.0;

    // Fetch the current temperature in F
    SHT31.read_f(&temp_f);
    
    // Report them both to the user
    return pass("%s (temp)", strfloat(temp_f, 0, 2));
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

    // Fetch the current temperature in F
    float temp_f = atof(token);
    
    // Store the new setpoint in EEPROM
    ee.setpoint_f = temp_f;

    // Tell the temperature controller what to use for a setpoint
    TempCtrl.new_setpoint_f(temp_f);

    // Put the system in setpoint mode
    SetpointModeMgr.start();

    // Report them both to the user
    return pass("%s (setpoint)", strfloat(temp_f, 0, 2));
}
//=========================================================================================================


//=========================================================================================================
// handle_transmit() - Transmits a telemetry packet via radio to the gateway
//=========================================================================================================
bool CSerialServer::handle_transmit()
{
    SHT31.read_f(&System.temp_f);
    Radio.transmit_telemetry();
    return pass();
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

        // Convert the token to float, in degrees F
        float temp_f = atof(token);

        // "sim temp off" means stop simulating
        if token_is("off") temp_f = CSHT31::SIM_TEMP_OFF;

        // Tell the sensor to simulate this room temp.  -99 degrees = "stop simulating"
        SHT31.simulate_temp(temp_f);

        // Tell the user that all is well.
        return pass("%s (sim_temp)", strfloat(temp_f, 0, 2));
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
        Display.fill();
        Led.set(WHITE);
        EEPROM.destroy();
        pass();
        delay(1000);
        System.reboot();
        return false;
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
    char encryption_key[20];
    const char* space = " ";

    const char run_mode_m[]   PROGMEM = "run_mode           : 0 - MANUAL";
    const char run_mode_s[]   PROGMEM = "run_mode           : 2 - SETPOINT";
    const char run_mode[]     PROGMEM = "run_mode           : %i";
    const char manual_index[] PROGMEM = "manual_index       : %i";
    const char setpoint[]     PROGMEM = "setpoint           : %i";
    const char orientation[]  PROGMEM = "orientation        : %i";
    const char is_servocal[]  PROGMEM = "is_servo_cal       : %i";
    const char servo_min[]    PROGMEM = "servo_min          : %i";
    const char servo_max[]    PROGMEM = "servo_max          : %i";
    const char notches[]      PROGMEM = "notches            : %i";
    const char tcm[]          PROGMEM = "tcm                : %i";
    const char deadband[]     PROGMEM = "deadband           : %s";
    const char network[]      PROGMEM = "network            : %i";
    const char node[]         PROGMEM = "node               : %i";
    const char encrypt[]      PROGMEM = "encrypt            : %s";

    // Get a handy reference to the EEPROM structure the caller wants to dump
    CEEPROM::data_t& ee = *(CEEPROM::data_t*)vp;

    // Fetch the encryption key as a nul-terminated string
    int key_size = sizeof(ee.encrypt);
    memcpy(encryption_key, ee.encrypt, key_size);
    encryption_key[key_size] = 0;

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
    replyf(notches,         ee.notches);
    replyf(tcm,             ee.tcm);
    replyf(deadband,        strfloat(ee.deadband, 0, 2));
    replyf(network,         ee.network);
    replyf(node,            ee.node);
    replyf(encrypt,         encryption_key);
    

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
    
    const char line_01 [] PROGMEM = "ee                          - Displays EEPROM contents";
    const char line_02 [] PROGMEM = "ee dirty                    - Displays EEPROM shadow RAM";
    const char line_03 [] PROGMEM = "ee destroy                  - Erases EEPROM";
    const char line_04 [] PROGMEM = "fwrev                       - Displays firmware revision";
    const char line_05 [] PROGMEM = "reboot                      - Soft reboots device";
    const char line_06 [] PROGMEM = "eeset is_servocal <value>   - Saves servo calibration flag";
    const char line_07 [] PROGMEM = "eeset notches <value>       - Save new notch count for temp ctrl";
    const char line_08 [] PROGMEM = "eeset tcm <value>           - Save new time-constant multiplier";
    const char line_09 [] PROGMEM = "eeset deadband <value>      - Save new temperature deadband size";
    const char line_10 [] PROGMEM = "eeset network <network_id>  - Save new network ID, 0 thru 255";
    const char line_11 [] PROGMEM = "eeset node <node_id>        - Save new node ID, 0 thru 1023";
    const char line_12 [] PROGMEM = "eeset encrypt <16_byte_key> - Save new 16 byte encryption key";
    const char line_13 [] PROGMEM = "sim temp <deg_F>            - Simulates the room temperature";
    const char line_14 [] PROGMEM = "temp                        - Reports the room temperature";
    const char line_15 [] PROGMEM = "setpoint <deg_F>            - Sets new temp control setpoint";
    const char line_16 [] PROGMEM = "ui <l | left>               - Simulate knob rotate left";
    const char line_17 [] PROGMEM = "ui <r | right>              - Simulate knob rotate right";
    const char line_18 [] PROGMEM = "ui <c | click>              - Simulate knob click";
    const char line_19 [] PROGMEM = "ui <lp | lpress>            - Simulate knob long press";
    const char line_20 [] PROGMEM = "clog <start [value] | stop> - Turn the current logger on or off";
    const char line_21 [] PROGMEM = "servo <pwm_position>        - Move the servo to the specified position";
    const char line_22 [] PROGMEM = "transmit                    - Transmit telemetry packet via radio";

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
    replyf(line_20);
    replyf(line_21);
    replyf(line_22);

    return pass();
}
//=========================================================================================================


//=========================================================================================================
// handle_nvset() - Handles the commands that store data into EEPROM
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

    if token_is("network")
    {
        ee.network = int(fvalue);
        EEPROM.write();
        return pass();
    }
    
    if token_is("node")
    {
        ee.node = int(fvalue);
        EEPROM.write();
        return pass();
    }

    if token_is("encrypt")
    {
        memcpy(ee.encrypt, value, sizeof(ee.encrypt));
        EEPROM.write();
        return pass();
    }


    // If we get here, there was a syntax error
    return fail_syntax();
}
//=========================================================================================================

