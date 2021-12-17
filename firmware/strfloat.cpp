#include <stdint.h>

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
const char* strfloat(float value, int8_t width, int8_t decimals)
{
    static char buffer[20];
    char        digit, * out = buffer;
    uint32_t    divisor, whole, part;
    int8_t      i, div_count;

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
    uint8_t current_length = out - buffer;

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
        return buffer;
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
    return buffer;
}
//=========================================================================================================


