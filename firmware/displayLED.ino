// LED matrix functions for BORC
// Updated 07/11/2021

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

  // set rotation
//  rotateDisplay(false);

  // set text brightness (0 = dark, 255 = full brightness)
  ledmatrix.setTextColor(255);

  // clear display
  ledmatrix.clear();
}

// =================================================================
// Rotate LED matrix based on flag argument
// =================================================================
void rotateDisplay(bool rotateDisplayFlag)
{
  if (rotateDisplayFlag == false)
    ledmatrix.setRotation(0); // 0 = upright, 2 = upside down, 1,3 = 90° rotated
  else if(rotateDisplayFlag == true)
    ledmatrix.setRotation(2); // 0 = upright, 2 = upside down, 1,3 = 90° rotated
}

// =================================================================
// Update LED matrix display based on argument
// =================================================================
void updateDisplay(char choice)
{
  // clear display
  ledmatrix.clear();
  
  // set cursor to position
  ledmatrix.setCursor(2,0);
 
  // print manual or automatic setpoint on display -----------------
  if(choice == 'M' || choice == 'S')
  {
    // convert setpoint to string
    displayText = String(setpoint);

    // display on screen
    ledmatrix.print(displayText);

    // check and display min/max setpoint line
    checkMinMax();
  }

  // print current room temp on display ----------------------------
  else if(choice == 'T')
  {
    // convert temp from float to int and rounding up
    int display_temp = (int)(temp_f + 0.5);

    // convert int to String
    displayText = String(display_temp);

    // display on screen
    ledmatrix.print(displayText);
  }

  // show calibration option ---------------------------------------
  else if(choice == 'C')
  {
    // create calibration display text
    displayText = "Ca";

    // display on screen
    ledmatrix.print(displayText);
  }

  // show rotate display option ------------------------------------
  else if(choice == 'R')
  {
    // create rotate display text
    displayText = "RD";

    // display on screen
    ledmatrix.print(displayText);
  }

  // show system reset display -------------------------------------
  else if(choice == 'X')
  {
    // blank out entire screen with white LEDs
    ledmatrix.fillRect(0,0, ledmatrix.width(), ledmatrix.height(), 255);
  }

  // fade out display ----------------------------------------------
  else if(choice == 'F')
  {
    // turn LEDs off
    toggleLED(0,0,0);

    // fade LEDs out in a gradual loop
    for(int i=200; i>=0; i=i-10)
    {
      // adjust brightness
      ledmatrix.setTextColor(i);

      // print current text on screen with reduced brightness
      ledmatrix.print(displayText);

      // give a small delay to make the fade out smooth
      delay(1);
    }

    // finally, turn off display
    ledmatrix.clear();
  }

  // clear display -------------------------------------------------
  else if(choice == '-')
  {
    // turn off display
    ledmatrix.clear();
  }
}

// =================================================================
// Check min/max setpoint position and display line
// =================================================================
void checkMinMax()
{
  // if maximum setpoint reached
  if(displayMax == true)
  {
    // show max line
    for (int i=0; i<256; i=i+10)
    {
      ledmatrix.drawLine(14,0, 14, 6, i);
      delay(1);
    }

    // wait for a bit
    delay(300);

    // fade out max line
    for (int i=254; i>=0; i=i-3)
    {
      ledmatrix.drawLine(14,0, 14, 6, i);
      delay(1);
    }

    // make sure line is faded out
    ledmatrix.drawLine(14,0, 14, 6, 0);

    // reset max setpoint display flag
    displayMax = false;
  }

  // if minimum setpoint reached
  if(displayMin == true)
  {
    // show min line
    for (int i=0; i<256; i=i+10)
    {
      ledmatrix.drawLine(0,0, 0, 6, i);
      delay(1);
    }

    // wait for a bit
    delay(300);

    // fade out min line
    for (int i=254; i>=0; i=i-3)
    {
      ledmatrix.drawLine(0,0, 0, 6, i);
      delay(1);
    }

    // make sure line is faded out
    ledmatrix.drawLine(0,0, 0, 6, 0);

    // reset min setpoint display flag
    displayMin = false;
  }
}


// =================================================================
