//=========================================================================================================
// display.h - Defines a generic wrapper for Adafruit's IS31FL3731 LED matrix driver library
//=========================================================================================================

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

class CLedMatrix
{
public:
    
    void init();

    
    void display(int);
    void display(const char*);
    void display(double);

    
    void rotate();

protected:

    void clearDisplay();

    

};




#endif
