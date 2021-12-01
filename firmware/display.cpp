//=========================================================================================================
// display.cpp - Manages all necessary commands needed to use the LED matrix display
//=========================================================================================================
#include "display.h"
#include "common.h"
#include "power_manager.h"
#include <stdio.h>
#include <Adafruit_IS31FL3731.h>      //https://github.com/adafruit/Adafruit_IS31FL3731

// initialize all library objects
static Adafruit_IS31FL3731_Wing ledmatrix;
CPowerManager leddisplay;

//=========================================================================================================
// init() - Called once to initialize this object
//=========================================================================================================
void CLedMatrix::init()
{
  // turn display's power on
  leddisplay.powerOn(LED_SCREEN_POWER_PIN);
  
  // initialize LED matrix
  ledmatrix.begin(LED_MATRIX_ADDRESS);

  // set rotation of the display (0 = upright, 2 = upside down, 1,3 = 90° rotated)
  ledmatrix.setRotation(2);
  
  // set text brightness (text, background) (0 = dark, 255 = full brightness)
  ledmatrix.setTextColor(255, 0);

  // set cursor to position
  ledmatrix.setCursor(2,0);

  // clear display
  ledmatrix.clear();
}

//=========================================================================================================

//=========================================================================================================
// display() - these routines will show any value on the display
//=========================================================================================================
void CLedMatrix::display(const char* text)
{
  // clear display
  ledmatrix.clear();

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
