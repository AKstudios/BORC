// Knob UX functions for BORC
// Updated 09/15/2021

// =================================================================
// Function to check knob status
// =================================================================
void checkKnobStatus()
{
  Serial.println("checking knob status");

  // If knob was rotated but not clicked ---------------------------
  if(knobRotateFlag == true && knobClickFlag == false && knobDirection != 0 && advancedOptions == false)
  {
    Serial.println("knob rotated");

    // If automatic mode is set
    if(manualMode == false)
    {
      Serial.println("setpoint mode");
   
      // set the display mode to manual (0-6)
      displayMode = 'M';

      // reset knob direction
      knobDirection = 0;
    }

    // if manual mode is set
    else if(manualMode == true)
    {
      Serial.println("manual mode");

      // set the display mode to setpoint (55-90 F)
      displayMode = 'S';
      
      // reset knob direction
      knobDirection = 0;
    }

    // reset knob rotate flag
    knobRotateFlag = false;

    // return back once done to avoid going into advanced options menu on first pass
    return;
  }

  // If the knob was clicked but not rotated -----------------------
  else if(knobRotateFlag == false && knobClickFlag == true && knobDirection == 0 && advancedOptions == false)
  {
    Serial.println("knob clicked");

    // If automatic mode is set
    if(manualMode == false)
    {
      Serial.println("setpoint mode");
   
      // set the display mode to manual (0-6)
      displayMode = 'M';

      // reset knob click flag
      knobClickFlag = false;
    }

    // if manual mode is set
    else if(manualMode == true)
    {
      Serial.println("manual mode");

      // set the display mode to setpoint (55-90 F)
      displayMode = 'S';

      // reset knob click flag
      knobClickFlag = false;
    }

    // keep the last millis time in memory
    last_time = millis();
    
    // if the button is still pressed
    while(digitalRead(KNOB_CLICK) == 0) 
    {
      // get current time to compare with last time
      current_time = millis();
      
      // set advanced menu options when knob held for 2-5 secs
      if(current_time - last_time >= 2000 && current_time - last_time <= 4000)
      {
        // toggle teal LED to indicate advanced options
        toggleLED(0,1,1);

        // if advanced options not set previously
        if(advancedOptions == false)
        {
          Serial.println("toggle advanced options");

          // enable set advanced options flag
          advancedOptions = true;

          // reset menu options counter back to 1
          menuOption = 1;

          // set display mode for advanced options
          setAdvancedOptionDisplayMode();

          // break out of while loop to display advanced options
          break;
        }
      }

      // if button held more than 4 seconds
      else if(current_time - last_time > 4000)
      {
        Serial.println("knob pressed too long!");

        // timeout and break out of loop
        break;
      }

      // if knob is no longer being held
      if(digitalRead(KNOB_CLICK) == 1)
      { 
        Serial.println("knob no longer pressed");

        // break out of loop
        break;
      }
    }

    // reset knob click flag
    knobClickFlag = false;

    // return back once done to avoid going into advanced options menu on first pass
    return;
  }

//  // Knob used, fade out display -----------------------------------
//  while (knobDirection == 0 && knobRotateFlag == true && knobClickFlag == false && advancedOptions == false)
//  {
//    wdt_reset();
//    current_time = millis();
//    if(current_time - last_time >= displayTimeout)
//    {
//      Serial.println("knob used, fading out display ...");
//      displayLED('f');
//      knobRotateFlag = false;
//    }
//  }
  

  // handle advanced options menu
  if(advancedOptions == true && knobClickFlag == false)
  {
    // set display mode for advanced options
    setAdvancedOptionDisplayMode();
    
    // knob was clicked on one of the options ----------------------
    if(knobClickFlag == true)
    {
      Serial.print(menuOption);
      Serial.println(" -- advanced knob clicked");
      
      // wait a bit
      delay(500);

      // Option 1 - Manual mode
      if (menuOption == 1)
      {
        Serial.println("manual mode selected");

        // toggle LED off
        toggleLED(0,0,0);

        // set manual mode
        manualMode = true;

        // change setpoint to 0
        setpoint = 0;

        // change setpoint max and min values
        setpointMaxValue = 6;
        setpointMinValue = 0;

        // set the display mode to manual (0-6)
        displayMode = 'M';

        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
        
        // return back to main
        return;
      }

      // Option 2 - setpoint mode
      else if (menuOption == 2)
      {
        Serial.println("setpoint mode selected");
        
        // toggle LED off
        toggleLED(0,0,0);

        // set setpoint mode
        manualMode = false;

        // change setpoint to 72
        setpoint = 72;

        // change setpoint max and min values
        setpointMaxValue = 90;
        setpointMinValue = 55;

        // set the display mode to setpoint (55-90 F)
        displayMode = 'S';

        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
        
        // return back to main
        return;
      }
  
      // Option 3 - Room temperature mode
      else if (menuOption == 3)
      {
        Serial.println("room temp mode selected");

        // toggle LED off
        toggleLED(0,0,0);

        // restore previously set display mode
        displayMode = _displayMode;

        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
                
        // return back to main
        return;
      }
  
      // Option 4 - calibration mode
      else if (menuOption == 4)
      {
        Serial.println("calibration mode selected");

        // calibrate servo
        calibrateServo();

        // set manual mode
        manualMode = true;

        // restore previously set display mode
        displayMode = _displayMode;

        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
        
        // return back to main
        return;
      }
  
      // Option 5 - Rotate display mode
      else if (menuOption == 5)
      {
        Serial.println("rotate display mode selected");
        
        // rotate display based on the set flag
        if(rotateDisplay == false)
          rotateDisplay(true);
        else if(rotateDisplay == true)
          rotateDisplay(false);

        // restore previously set display mode
        displayMode = _displayMode;

        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
        
        // return back to main
        return;
      }
  
      // Option 6 - System reset mode
      else if (menuOption == 6)
      {
        Serial.println("SYSTEM RESET!");

        // toggle orange LED
        toggleLED(1,1,0);

        // update display with system reset mode
        updateDisplay('X');

        // reset system
        systemReset(true);

        // just in case system reset fails
        // reset advanced options
        advancedOptions = false;

        // reset knob click flag
        knobClickFlag = false;

        // reset knob rotate flag
        knobRotateFlag = false;

        // reset menu option back to 0
        menuOption = 0;
        
        // return back to main
        return;
      }
    }
  }
}

// =================================================================
// Function to check and set which advanced menu option to display
// =================================================================
void setAdvancedOptionDisplayMode()
{
  // Advanced options menu -----------------------------------------
  if(advancedOptions == true)
  {
    // Option 1 - Manual mode
    if (menuOption == 1)
      displayMode = 'M';

    // Option 2 - Setpoint mode
    else if (menuOption == 2)
      displayMode = 'S';

    // Option 3 - Room temp
    else if (menuOption == 3)
      displayMode = 'T';

    // Option 4 - Calibration mode
    else if (menuOption == 4)
      displayMode = 'C';

    // Option 5 - Rotate display mode
    else if (menuOption == 5)
      displayMode = 'R';

    // Option 6 - System reset mode
    else if (menuOption == 6)
      displayMode = 'X';
  }

  // redundant check
  else
    return;
}

// =================================================================
