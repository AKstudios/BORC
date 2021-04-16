// LED matrix functions for BORC
// Updated 04/03/2021

// =================================================================
// Initialize LED matrix
// =================================================================
void initializeLEDmatrix()
{
  // turn on LED matrix
  controlDevices(LED_SCREEN_POWER_PIN, HIGH);

  // initialize matrix
  if(!ledmatrix.begin(LED_MATRIX_ADDRESS))
    errorCode |= (1<<LED_DRV_ERR);
  else
    errorCode &= ~(1<<LED_DRV_ERR);

  ledmatrix.setRotation(0); // 0 = upright, 2 = upside down, 1,3 = 90° rotated
  ledmatrix.setTextColor(255);
  ledmatrix.clear();
  ledmatrix.setCursor(2,0);
}

// =================================================================
// Display LED matrix
// =================================================================
void displayLED(char choice)
{
  // Initialize LED display again only if woken up by knob interrupt first time
//  if(knobCounter <= 10)   // to account for initial bounces in rotary encoder
//  {
//    knobCounter = 11; // set it arbritarily higher so don't have to initialize display again
//    initializeLEDmatrix();
//  }

    ledmatrix.setTextColor(255);
    ledmatrix.setCursor(2,0);
    ledmatrix.clear();

  // Turn on LED matrix only if it was previously turned off
  if(digitalRead(LED_SCREEN_POWER_PIN == LOW))
    controlDevices(LED_SCREEN_POWER_PIN, HIGH);

  // print setpoint on LED display ---------------------------------
  if(choice == 's')
  {
    displayText = String(setpoint);
    ledmatrix.print(displayText);
    
    if(displayMax == true)
    {
      for (int i=0; i<256; i=i+10)
      {
        ledmatrix.drawLine(14,0, 14, 6, i);
        delay(1);
      }
      delay(500);
    
      for (int i=254; i>=0; i=i-3)
      {
        ledmatrix.drawLine(14,0, 14, 6, i);
        delay(1);
      }
      ledmatrix.drawLine(14,0, 14, 6, 0);
      displayMax = false;
    }
  
    if(displayMin == true)
    {
      for (int i=0; i<256; i=i+10)
      {
        ledmatrix.drawLine(0,0, 0, 6, i);
        delay(1);
      }
      delay(500);
    
      for (int i=254; i>=0; i=i-3)
      {
        ledmatrix.drawLine(0,0, 0, 6, i);
        delay(1);
      }
      ledmatrix.drawLine(0,0, 0, 6, 0);
      displayMin = false;
    }
  }

  // print current room temp on LED display ------------------------
  else if(choice == 't')
  {
    int display_temp = (int)(temp_f + 0.5); // round up to integer
    displayText = String(display_temp);
    ledmatrix.print(displayText);
  }

  // show manual mode menu option ----------------------------------
  else if(choice == 'M')
  {
    displayText = "Ma";
    ledmatrix.print(displayText);
  }

  // show calibration menu option ----------------------------------
  else if(choice == 'C')
  {
    displayText = "Ca";
    ledmatrix.print(displayText);
  }

  // show setpoint mode menu option --------------------------------
  else if(choice == 'S')
  {
    displayText = "Se";
    ledmatrix.print(displayText);
  }

  // show manual controls on screen --------------------------------
  else if(choice == '<')
  {
    ledmatrix.setCursor(0,0);
    displayText = "<";
    ledmatrix.print(displayText);
        ledmatrix.setCursor(9,0);
    displayText = ">";
    ledmatrix.print(displayText);
  }

  // fade out display ----------------------------------------------
  else if(choice == 'f')
  {
    toggleLED(0,0,0); // turn LEDs off
    for(int i=200; i>=0; i=i-10)
    {
      ledmatrix.setTextColor(i);  // adjust brightness
      if (manualMode == true)
      {
        ledmatrix.setCursor(0,0);
        displayText = "<";
        ledmatrix.print(displayText);
        ledmatrix.setCursor(9,0);
        displayText = ">";
        ledmatrix.print(displayText);
      }
      else
      {
        ledmatrix.setCursor(2,0);
        ledmatrix.print(displayText);
      }
      delay(1);
    }
    ledmatrix.clear();  // turn off display
  }

  // clear display -------------------------------------------------
  else if(choice == 'x')
  {
    ledmatrix.clear();  // turn off display
  }
}

// =================================================================
