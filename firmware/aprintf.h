#ifndef _APRINTF_H_
#define _APRINTF_H_
#include <stdint.h>

// Converts a float into an ASCII string
const char* strfloat(float value, int8_t width = 0, int8_t decimals = 2, char* buffer = nullptr);

// Mimics sprintf()
int     asprintf(char* buffer, const char* fmt, ...);

// Mimics printf()
void    aprintf(const char* fmt, ...);

#endif

