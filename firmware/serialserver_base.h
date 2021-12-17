//=========================================================================================================
// serialserver_base.h - The base class for a serial input based command server
//=========================================================================================================
#ifndef _SERIALSERVER_BASE_H_
#define _SERIALSERVER_BASE_H_
#include <stdint.h>
#include <stdarg.h>

class CSerialServerBase
{
public:

    // Constructor
    CSerialServerBase() { reset(); }

    // Call this to throw away any partially recieved messages
    void    reset();

    // State machine.  Either call this often, or call it with blocking = true
    void    execute(bool blocking = false);

protected:

    // This gets called to check for incoming characters.  Over-ride this
    virtual int   is_data_available() = 0;

    // This gets called to read an incoming character.  Over-ride this
    virtual int   read() = 0;

    // This gets called whenever a new command is received. Over-ride this
    virtual void  on_command(const char* command) = 0;

    // This gets called to write a line of text to the output.  Over-ride this
    virtual void  writeln(const char* text) = 0;

protected:

    // Message handlers call this to fetch the next available token.  Returns false is none available
    bool    get_next_token(const char** p_token);

    // Message handlers call these to indicate pass or fail
    bool    pass(const char* fmt = nullptr, ...);
    bool    fail(const char* fmt = nullptr, ...);
    bool    fail_syntax();

    // Lowest level methods for replying to a command
    void    reply(const char* status, const char* fmt, va_list& va);
    void    replyf(const char* fmt, ...);

    // Compares two strings. s2 can be in either Flash or RAM
    bool    compare_token(const char* s1, const char* s2);

private:

    // This gets called when carriage-return or linefeed is received
    void    handle_new_message();

    // This is our incoming message
    char    m_message[64];

    // This is where the next input byte gets stored
    char*   m_input;

    // When "get_next_token()" is called, this points to the 1st char of the next token
    char*   m_next_token;

    // Number of bytes free in the input buffer
    uint8_t m_free_remaining;

    // This is the prefix that gets output prior to all of our messages
    const char* m_prefix = "$$>> ";

};



#endif
