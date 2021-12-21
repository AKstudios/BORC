//=========================================================================================================
// aprintf() - This file contains implementations of strfloat, aprintf(), and asprintf()
//=========================================================================================================
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <arduino.h>

//=========================================================================================================
// strfloat() - A routine for converting a float to a displayable string, optimized for smaller 
//              CPUs.
// 
//  Passed:  value    = The value to translate into a string
//
//           width    = The number of characters the digits before the decimal should take up.  If the value
//                      is too small to fill the width, it will be pre-padded with spaces.  If the
//                      value won't fit into that width, the width will be ignored.   A width of 0 means
//                      "never pad".
//                      Default width is 0
// 
//           decimals = The number of places after the decimal point to display.  Value will be 
//                      rounded to the specified number of decimals.  0 is valid.
//                      Default decimals is 1
// 
//  Returns: a const char* to a static buffer containing the formatted string.
//
//  Notes:
//    The output buffer is 20 bytes long.  Don't overflow it!  Using a static buffer here makes this
//    routine convenient to use, but makes it unsuitable for use in multi-threaded environment.  
//
// 
// To test:
//     float value = 34.156;
//     printf("The value is %s\n", strfloat(value));
//=========================================================================================================
const char* strfloat(float value, int8_t width, int8_t decimals, char* user_buffer)
{
    static char local_buffer[20];
    char        digit, *buffer_start, *out;
    uint32_t    divisor, whole, part;
    int8_t      i, div_count;

    // Point either to the caller provided buffer, or to our internal one
    buffer_start = out = (user_buffer == nullptr) ? local_buffer : user_buffer;

    // A list of decimal divisors
    const uint32_t divs[] = { 1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10 };

    // How many divisors are there?
    div_count = sizeof(divs) / sizeof(divs[0]);

    // If value is negative, make it positive
    if (value < 0)
    {
        *out++ = '-';
        value = -value;
    }

    // If the user doesn't want decimals, then we round the whole 
    if (decimals == 0)
        whole = (uint32_t)(value + .5);

    // Otherwise, we round the part afte the decimals
    else
    {
        whole = (uint32_t)(value);
        value -= whole;
        part = (uint32_t)(value * divs[div_count - decimals] + .5);
    }

    // We've not yet output our first digit
    bool first_digit_seen = false;

    // Loop through each divisor
    for (i = 0; i < div_count; ++i)
    {
        digit = '0';
        divisor = divs[i];

        // How many times can we subtract this divisor from the value?
        while (whole >= divisor)
        {
            ++digit;
            whole -= divisor;
        }

        // If this is non-zero, then we've seen the first digit
        if (digit != '0') first_digit_seen = true;

        // If we've seen the first digit or this is non-zero, output the digit
        if (first_digit_seen || digit != '0') *out++ = digit;
    }

    // When we get here, whole is guaranteed to be less than 10.  Output the digit
    *out++ = '0' + whole;

    // How many bytes have we output so far?
    uint8_t current_length = out - buffer_start;

    // If we need to pad with spaces to fulfill the caller's width requirement...
    if (width && current_length < width)
    {
        // Back up to point to the last character we output
        --out;

        // How many leading spaces do we need?
        uint8_t shift = width - current_length;

        // Point to the last character we output
        char* src = out;

        // Point to where we're going to move them to
        char* dst = out + shift;

        // And point to where the decimal point goes
        out += (shift + 1);

        // Copy source to destination, moving bytes forward in the buffer
        while (current_length--) *dst-- = *src--;

        // Now fill in the leading spaces
        while (shift--) *dst-- = ' ';
    }

    // If we're not outputting decimals, we're done!
    if (decimals == 0)
    {
        *out = 0;
        return buffer_start;
    }

    // Output the decimal point
    *out++ = '.';

    // Point to the divisor one entry prior the one we need
    const uint32_t* div_ptr = divs + div_count - decimals;

    // Loop through each divisor
    while (--decimals)
    {
        digit = '0';
        divisor = *++div_ptr;

        // How many times can we subtract this divisor from the value?
        while (part >= divisor)
        {
            ++digit;
            part -= divisor;
        }

        // Output the digit
        *out++ = digit;
    }

    // When we get here, part is guaranteed to be less than 10.  Output the digit
    *out++ = '0' + part;

    // Terminate the buffer and we're done
    *out = 0;
    return buffer_start;
}
//=========================================================================================================


//=========================================================================================================
// avsprintf() - Mimics vsprintf()
//=========================================================================================================
int avsprintf(char* buffer, const char* fmt, va_list& va)
{
    char c, *out = buffer;
    const char* in = fmt;
    char format[20], * fout;


    // Loop through every character of the input...
    while (true)
    {
        // Fetch the next character
        c = *in++;

        // If we it's the string terminator, we're done
        if (c == 0)
        {
            *out = 0;
            return out - buffer;
        }

        // If it's not a percent sign, we're done
        if (c != '%')
        {
            *out++ = c;
            continue;
        }

        // If the next character is nul, just output it
        if (*in == 0)
        {
            *out++ = '%';
            continue;
        }

        // If the next character is %, swallow the first one
        if (*in == '%')
        {
            *out++ = *in++;
            continue;
        }

        // Point to the buffer that will hold our format string
        fout = format;

        // Our format should always begin with a '%'
        *fout++ = '%';

        // Read in the numeric portion of the format specifier
        while ((*in >= '0' && *in <= '9') || *in == '-' || *in == '.') *fout++ = *in++;

        // Find the value of the ASCII characters before and after the decimal point
        int pre_decimal = 0;
        int post_decimal = 0;
        pre_decimal = atoi(format + 1);
        char* dp = strchr(format, '.');
        if (dp) post_decimal = atoi(dp + 1);

        // Read in the non-numeric portion of the format specifier
        int fmt_char = 0;
        --in;
        while ((c = *++in) != 0)
        {
            *fout++ = c;

            if (c == 'i' || c == 'd' || c == 'u' || c == 'f' || c == 'c' || c == 's')
            {
                ++in;
                fmt_char = c;
                break;
            }
        }


        // Terminated the format-specifier
        *fout = 0;

        // If we're formatting a string, fetch the string pointer and make it so
        if (fmt_char == 's')
        {
            char* s_value = va_arg(va, char*);
            out += snprintf(out, 100, format, s_value);
            continue;
        }

        // If we're formating a float, fetch the float and make it so
        if (fmt_char == 'f')
        {
            float f_value = (float)va_arg(va, double);
            strfloat(f_value, pre_decimal, post_decimal, out);
            out = strchr(buffer, 0);
            continue;
        }

        // Otherwise, we're formatting some form of an integer
        int i_value = va_arg(va, int);
        out += snprintf(out, 100, format, i_value);
    }

}
//=========================================================================================================



//=========================================================================================================
// asprintf() - This mimics ordinary sprintf(), including floating-point support
//=========================================================================================================
int asprintf(char* buffer, const char* fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    int length = avsprintf(buffer, fmt, va);
    va_end(va);
    return length;
}
//=========================================================================================================


//=========================================================================================================
// aprintf() - Mimics printf()
//=========================================================================================================
void aprintf(const char* fmt, ...)
{
    char buffer[200];
    va_list va;
    va_start(va, fmt);
    avsprintf(buffer, fmt, va);
    va_end(va);
    Serial.print(buffer);
}
//=========================================================================================================
