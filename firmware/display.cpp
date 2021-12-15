//=========================================================================================================
// display.cpp - Manages all necessary commands needed to use the LED matrix display
//=========================================================================================================
#include "globals.h"
#include <stdio.h>
#include <Adafruit_IS31FL3731.h>      //https://github.com/adafruit/Adafruit_IS31FL3731

// initialize all library objects
static Adafruit_IS31FL3731_Wing ledmatrix;

//=========================================================================================================
// init() - Called once to initialize this object
//=========================================================================================================
void CLedMatrix::init()
{
  // turn display's power on
  PowerMgr.powerOn(LED_SCREEN_POWER_PIN);
  
  // initialize LED matrix
  ledmatrix.begin(LED_MATRIX_ADDRESS);

  // set upright system display orientation
  set_orientation(true);
  
  // set text brightness (text, background) (0 = dark, 255 = full brightness)
  ledmatrix.setTextColor(255, 0);

  // set cursor to position
  ledmatrix.setCursor(2,0);

  // clear display
  ledmatrix.clear();
}
//=========================================================================================================


//=========================================================================================================
// rotate() - rotate the display between upright and upside down
//=========================================================================================================
void CLedMatrix::set_orientation(bool flag)
{ 
  // this sets orientation of the display (0 = upright, 2 = upside down, 1,3 = 90° rotated)
  int orientation = flag ? 2 : 0;
  
  // flip the screen orientation..
  ledmatrix.setRotation(orientation);

  // .. and redisplay the current output
  display(m_current_output);
}
//=========================================================================================================


//=========================================================================================================
// display() - these routines will show any value on the display
//=========================================================================================================
void CLedMatrix::display(const char* text)
{
  // save current output
  m_current_output[0] = text[0];
  m_current_output[1] = text[1];
  m_current_output[2] = 0;
  
  // clear display
  ledmatrix.clear();
  
  // set cursor to position
  ledmatrix.setCursor(2,0);

  // print something on display
  ledmatrix.print(text);
}

// display number
void CLedMatrix::display(int number)
{
  // create a buffer to store a number
  char buffer[10];

  // convert number into a formated char array which always has 2 chars which is the display's limit
  sprintf(buffer, "%2d", number);

  // push buffer to display
  display(buffer);
}

// display float/double values
void CLedMatrix::display(double value)
{
  // create a buffer to store a number
  char buffer[10];

  // convert a rounded up double/float into a formated char array which always has 2 chars which is the display's limit
  sprintf(buffer, "%2d", (int)(value+.5));

  // push buffer to display
  display(buffer);
}
//=========================================================================================================


//=========================================================================================================
// clear() - this will clear the display
//=========================================================================================================
void CLedMatrix::clear()
{
  ledmatrix.clear();
}
//=========================================================================================================


//=========================================================================================================
// fill() - this will fill up the display - turn all LEDs on by drawing a rectangle and filling it
//=========================================================================================================
void CLedMatrix::fill()
{
  ledmatrix.fillRect(0,0, ledmatrix.width(), ledmatrix.height(), 255);
}
//=========================================================================================================
