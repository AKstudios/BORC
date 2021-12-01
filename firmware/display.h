//=========================================================================================================
// display.h - Defines a generic wrapper for Adafruit's IS31FL3731 LED matrix driver library
//=========================================================================================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

//=========================================================================================================
// CLedMatrix() - A class that manages the LED display that can print text, numbers and float                
//=========================================================================================================
class CLedMatrix
{
public:

    // Call once to initialize the knob
    void init();
    
    // these routines will show any value on the display
    void display(int);
    void display(const char*);
    void display(double);
};

//=========================================================================================================
#endif
