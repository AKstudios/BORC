#include "display.h"
#include "common.h"
#include <stdio.h>
#include <Adafruit_IS31FL3731.h>      //https://github.com/adafruit/Adafruit_IS31FL3731

static Adafruit_IS31FL3731_Wing ledmatrix;

void CLedMatrix::init()
{
  pinMode(LED_SCREEN_POWER_PIN, OUTPUT);
  digitalWrite(LED_SCREEN_POWER_PIN, HIGH);
  
  ledmatrix.begin(LED_MATRIX_ADDRESS);

  // set text brightness (0 = dark, 255 = full brightness)
  ledmatrix.setTextColor(255, 0);

  // clear display
  ledmatrix.clear();

  // set cursor to position
  ledmatrix.setCursor(2,0);
}


void CLedMatrix::display(int number)
{
  char buffer[10];

  sprintf(buffer, "%2d", number); // always prints 2 chars

  display(buffer);
}


void CLedMatrix::display(const char* text)
{
  
  
  delay(100);

  // set text brightness (0 = dark, 255 = full brightness)
  ledmatrix.setTextColor(255, 0);
  
  // set cursor to position
  ledmatrix.setCursor(2,0);

  ledmatrix.clear();
  delay(100);

  ledmatrix.print(text);
  delay(100);
}


void CLedMatrix::display(double value)
{
  char buffer[10];

  sprintf(buffer, "%2d", (int)(value+.5));

  display(buffer);
}
