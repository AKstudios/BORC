#ifndef _SERIALSERVER_H_
#define _SERIALSERVER_H_
#include "serialserver_base.h"
#include <Arduino.h>


//=========================================================================================================
// Serial Server - Handles incoming commands from the serial port
//=========================================================================================================
class CSerialServer : public CSerialServerBase
{

protected:

    // Whenever a command comes in, this top-level handler gets called
    void    on_command(const char* command);

    // ---------------  Handlers for specific commands ------------------
    // ------------  The bool return values are meaningless  ------------
    bool    handle_fwrev();
    bool    handle_nv();
    bool    handle_nvset();
    bool    handle_reboot();
    bool    handle_help();
    // ------------------------------------------------------------------

    void    show_nv(void*);

// Directly use the Serial object to perform basic I/O functions
protected:

    int is_data_available()
    {
        return Serial.available();
    }

    int read()
    {
        return Serial.read();
    }

    void writeln(const char* text)
    {
        Serial.println(text);
    }

};
//=========================================================================================================


#endif