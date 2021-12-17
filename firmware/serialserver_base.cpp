#include <arduino.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "serialserver_base.h"


//=========================================================================================================
// copy_from_flash() - If the specified source pointer is in flash, copies the string from source
//                     to destination and returns true.
// 
//                     If the source pointer pointed to a location in RAM, we return false
//=========================================================================================================
#ifdef __AVR__
static bool copy_from_flash(char* dest, const char* src, uint16_t size)
{
    while (*src && size--)
    {
        char data = pgm_read_byte_near(src);
        if (data != *src) return false;
        *dest++ = data;
        ++src;
    }

    // Make sure the end of the string is terminated
    *dest = 0;

    // Tell the caller that his source pointer pointed to flash memory
    return true;
}
#endif
//=========================================================================================================


//=========================================================================================================
// reset() - Throws away any partially received messages
//=========================================================================================================
void CSerialServerBase::reset()
{
    // Reset the message pointer back to the beginning of the buffer
    m_input = m_message;

    // And we now have the entire buffer free for incoming data
    m_free_remaining = sizeof(m_message) - 1;
}
//=========================================================================================================



//=========================================================================================================
// execute() - State machine: Call this often, or with blocking set to true
// 
// If blocking is true:
//     Virtual function read() should block until it has data to return
//     If the input stream closes, read() should return -1
//     If read() returns -1, this routine will return
//=========================================================================================================
void CSerialServerBase::execute(bool blocking)
{
    int count = 0;

    // If we're not blocking, then make sure there is data available to read
    if (!blocking)
    {
        // Are there any bytes available to read on the serial port?
        count = Serial.available();

        // If there are no characters available to read, we're done
        if (count == 0) return;
    }

    // For each character available
    while (blocking || count--)
    {
        // Fetch the character
        int c = read();

        // If the read returned a negative value, it means it's input stream has no data
        if (c < 0) break;

        // Convert tabs to spaces
        if (c == 9) c = 32;

        // Handle backspace
        if (c == 8)
        {
            if (m_input > m_message)
            {
                --m_input;
                ++m_free_remaining;
            }
            continue;
        }

        // Handle both carriage-return and linefeed
        if (c == 13 || c == 10)
        {
            // If the message buffer is empty, ignore it
            if (m_input == m_message) continue;

            // Nul-terminate the message
            *m_input = 0;

            // Go see if the message needs to be handled
            handle_new_message();

            // Reset back to an empty message buffer
            reset();

            // If we're not blocking, let the other state machines have a turn at the CPU
            if (!blocking) break;

            // We're blocking: go fetch the next character
            continue;
        }

        // If there's room to add this character to the input buffer, make it so
        if (m_free_remaining)
        {
            *m_input++ = c;
            --m_free_remaining;
        }
    }
}
//=========================================================================================================

  


//=========================================================================================================
// handle_new_message() - Parse out the first token from a newly arrives message and potentially
//                        call the message handler
//
// On Entry:  m_message = Null terminated character string
//=========================================================================================================
void CSerialServerBase::handle_new_message()
{
    // Point to our input message
    char* in = m_message;

    // Skip over leading spaces
    while (*in == ' ') ++in;

    // If we've hit the end of the message, it was just spaces... ignore the message
    if (*in == 0) return;

    // This is where the first token begins
    const char* first_token = in;

    // Skip past the first token, converting to lowercase as we go
    --in;
    while (true)
    {
        // Fetch the next input character
        char c = *++in;

        // If we've hit the end of the token, break out of the loop
        if (c == ' ' || c == 0) break;

        // Convert uppercase to lowercase
        if (c >= 'A' && c < 'Z') *in = c + 32;
    }

    // If there are spaces after the first token, skip over them
    while (*in == ' ') *in++ = 0;

    // Make m_next_token point to the start of our first command parameter
    m_next_token = in;

    // Call the top level command handler
    on_command(first_token);

}
//=========================================================================================================



//=========================================================================================================
// get_next_token() - Provides a pointer to the next token if there is one
// 
// Passed:   A pointer to the user's char* that he wants pointed to the token
// 
// Returns:  true if there was a token available, otherwise false
// 
// On Exit:  The caller's char* points to the next token (or to a null byte if no token available).
//           If the original token was in quotation marks, they have been stripped.
//           A quoted token has letter-case and internal spaces preserved.
//           An unquoted token is always converted to lowercase.
//
// Notes: This routine always leaves m_token pointing to the first byte of the following token
//=========================================================================================================
bool CSerialServerBase::get_next_token(const char** p_retval)
{
    char c;

    // Get a pointer to the start of the token
    char* in = m_next_token;

    // If there isn't a next token available, tell the caller
    if (*in == 0)
    {
        *p_retval = in;
        return false;
    }

    // Does this token begin with a quote-mark?
    bool in_quotes = (*in == 34);

    // If the token begins with a quote-mark, skip over it
    if (in_quotes) ++in;

    // One way or another, this is the start of the token we'll hand to the caller
    *p_retval = in;

    // If it was just a lone quote-mark, there's no token
    if (*in == 0)
    {
        m_next_token = in;
        return false;
    }

    // Bump back by 1 so that the scanning loops below can always point to the character being examined
    --in;

    // If we're in quotation marks, search for the closing quote-mark
    if (in_quotes) while (true)
    {
        // Fetch the next character
        c = *++in;

        // If this was the closing quote-mark, convert to a space, and we're at the end of the token
        if (c == 34)
        {
            *in = ' ';
            break;
        }

        // A nul-byte always terminates the token
        if (c == 0) break;
    }

    // Otherwise, we're not in quotes, so search for a space at the end of the token
    else while (true)
    {
        // Fetch the next character
        c = *++in;

        // If we hit a space or a null, it's the end of the token
        if (c == ' ' || c == 0) break;

        // Convert to lower case as neccessary
        if (c >= 'A' && c <= 'Z') *in += 32;
    }

    // We're at the end of the token.  Scan past any spaces
    while (*in == ' ') *in++ = 0;

    // And this is where the next scan for tokens will begin
    m_next_token = in;

    // Tell the caller he has a token available
    return true;
}
//=========================================================================================================



//=========================================================================================================
// pass() - A printf-style function that allows a derives class to report success
//=========================================================================================================
bool CSerialServerBase::pass(const char* fmt, ...)
{
    va_list args;
    if (fmt) va_start(args, fmt);
    reply("OK", fmt, args);
    return true;
}
//=========================================================================================================



//=========================================================================================================
// fail() - A printf-style function that allows a derives class to report success
//=========================================================================================================
bool CSerialServerBase::fail(const char* fmt, ...)
{
    va_list args;
    if (fmt) va_start(args, fmt);
    reply("FAIL", fmt, args);
    return false;
}
//=========================================================================================================


//=========================================================================================================
// fail_syntax() - A convenience method for reporting a syntax error
//=========================================================================================================
bool CSerialServerBase::fail_syntax()
{
    va_list args;
    reply("FAIL SYNTAX", nullptr, args);
    return false;
}
//=========================================================================================================



//=========================================================================================================
// reply() - A printf-style function that outputs formatted strings
//=========================================================================================================
void CSerialServerBase::reply(const char* status, const char* fmt, va_list& args)
{
    const int buffer_size = 100;
    char buffer[buffer_size], *out = buffer;

    // If this is an AVR chip and fmt points to flash, fetch a RAM copy
    #ifdef __AVR__
    char local_fmt[buffer_size];
    if (fmt && copy_from_flash(local_fmt, fmt, buffer_size))
    {
        fmt = local_fmt;
    }
    #endif

    // Copy our prefix into the buffer
    const char* in = m_prefix;
    while (*in) *out++ = *in++;

    // Place the status into the buffer
    in = status;
    while (*in) *out++ = *in++;

    // If the caller provided data to be output...
    if (fmt)
    {
        // Add a space separator after the "OK"
        *out++ = ' ';

        // Find out how much space is free in the buffer
        int free_space = sizeof(buffer) - (out - buffer);

        // Print the caller's printf-style argument list to the buffer
        out += vsnprintf(out, free_space, fmt, args);

        // Clean up the variable argument buffer
        va_end(args);
    }

    // Make sure the buffer is terminated
    *out = 0;

    // Write this line of text to the output
    writeln(buffer);
}
//=========================================================================================================



//=========================================================================================================
// replyf() - A printf-style function that outputs formatted strings
//=========================================================================================================
void CSerialServerBase::replyf(const char* fmt, ...)
{
    va_list args;
    char null_byte = 0, *nothing = &null_byte;

    // Get a pointer to the first argument
    va_start(args, fmt);

    // And send the reply
    reply(nothing, fmt, args);
}
//=========================================================================================================


//=========================================================================================================
// compare_token() - Compares string at s1 to string at s2 and returns true if they match
//                   s2 may be in either Flash or RAM
//=========================================================================================================
bool CSerialServerBase::compare_token(const char* s1, const char* s2)
{
    // Compare, assuming both are in RAM
    if (strcmp(s1, s2) == 0) return true;

    #ifdef __AVR__
    if (strcmp_P(s1, s2) == 0) return true;
    #endif

    // If we get here, the strings don't match
    return false;
}
//=========================================================================================================

