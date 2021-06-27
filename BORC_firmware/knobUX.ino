// Knob UX functions for BORC
// Updated 04/19/2021

// =================================================================
// Function to check knob status
// =================================================================
void checkKnobStatus()
{
  Serial.println("checking knob status");
  // Knob rotated --------------------------------------------------
  if(knobFlag == true && knobClickFlag == false && WDTflag == false && knobDirection != 0)
  {
    Serial.println("knob rotated");
    if(manualMode == false)
    {
      Serial.println("setpoint mode true");
      displayLED('s');
      toggleLED(0,0,0); // turn LEDs off
      knobDirection = 0;    // reset knob direction
    }

    else if(manualMode == true)
    {
      Serial.println("manual mode true");
      displayLED('<');
      toggleLED(0,0,0); // turn LEDs off
      runServo();
      knobDirection = 0;    // reset knob direction
    }
    last_time = millis();
  }

  // Knob clicked --------------------------------------------------
  else if(knobFlag == true && knobClickFlag == true && WDTflag == false && knobDirection == 0)
  {
    Serial.println("knob clicked");
    last_time = millis();
    while(digitalRead(KNOB_CLICK) == 0) // button still pressed
    {
      current_time = millis();
      
      // display setpoint when button tapped once
      if(current_time - last_time >= 0 && current_time - last_time <= 1500)
      {
        if(manualMode == false)
        {
          toggleLED(0,0,0); // turn LEDs off
          displayLED('s');
        }
        else if(manualMode == true)
        {
          toggleLED(0,0,0); // turn LEDs off
          displayLED('<');
        }
      }

      // display advanced menu when knob held for 1-4 secs
      if(current_time - last_time >= 1500 && current_time - last_time <= 3500)
      {
        toggleLED(0,1,1); // toggle teal LED
        if(advancedOptions == false)
        {
          Serial.println("toggle advanced options");
          advancedOptions = true;
          delay(1000);
          break;
        }
      }

      // if button held more than 4 seconds, timeout
      else if(current_time - last_time > 3500)
      {
        Serial.println("knob pressed too long!");
        break;
      }

      // if knob is no longer being held, exit
      if(digitalRead(KNOB_CLICK) == 1)
      { 
        Serial.println("knob no longer pressed");
        break;
      }
    }
    last_time = millis();
    knobClickFlag = false;
  }

  // Reset if nothing found ----------------------------------------
  else
  {
    knobFlag == false;
    knobClickFlag = false;
    WDTflag == false;
    knobDirection = 0;
    wdt_reset();
  }

  // Knob used, fade out display -----------------------------------
  while (knobDirection == 0 && knobFlag == true && knobClickFlag == false && advancedOptions == false)
  {
    wdt_reset();
    current_time = millis();
    if(current_time - last_time >= displayTimeout)
    {
      Serial.println("knob used, fading out display ...");
      displayLED('f');
      knobFlag = false;
    }
  }
  
  // Advanced options menu -----------------------------------------
  while(advancedOptions == true)
  {
    current_time = millis();

    // Option 0 - Show room temp
    if (menuOption == 0)
      displayLED('t');

    // Option 1 - Manual mode
    else if (menuOption == 1)
      displayLED('M');

    // Option 2 - setpoint mode
    else if (menuOption == 2)
      displayLED('S');

    // Option 3 - Calibration mode
    else if (menuOption == 3)
      displayLED('C');

    // Option 4 - Rotate display mode
    else if (menuOption == 4)
      displayLED('R');

    // Option 5 - System reset mode
    else if (menuOption == 5)
      displayLED('X');


    // knob was clicked on one of the options ----------------------
    if(digitalRead(KNOB_CLICK) == 0)
    {
      Serial.println(menuOption);
      Serial.println("advanced knob clicked");
      if (menuOption == 0)  // Option 0 - room temp
      {
        Serial.println("room temp");
        toggleLED(0,0,0); // toggle LED off
        
        advancedOptions = false;
        manualMode = false;
        
        delay(500);
        displayLED('f');
        break;
      }

      else if (menuOption == 1)  // Option 1 - Manual mode
      {
        Serial.println("manual mode set");
        displayLED('<');
        toggleLED(0,0,0); // toggle LED off
        
        advancedOptions = false;
        manualMode = true;

        delay(500);
        displayLED('f');
        break;
      }
  
      // Option 2 - setpoint mode
      else if (menuOption == 2)
      {
        Serial.println("setpoint mode set");
        setpoint = 72;  // reset setpoint to 72
        displayLED('s');
        toggleLED(0,0,0); // toggle LED off
        
        advancedOptions = false;
        manualMode = false;

        delay(500);
        displayLED('f');
        break;
      }
  
      // Option 3 - Calibration mode
      else if (menuOption == 3)
      {
        Serial.println("calibration mode set");
        displayLED('C');
        toggleLED(0,0,0); // toggle LED off
        delay(500);
        calibrateServo();
              
        advancedOptions = false;
        manualMode = false;

        displayLED('f');
        break;
      }
  
      // Option 4 - Rotate display mode
      else if (menuOption == 4)
      {
        Serial.println("rotate display set");
        rotateDisplay(true);
        displayLED('R');
        toggleLED(0,0,0); // toggle LED off
        
        advancedOptions = false;
        manualMode = false;

        delay(500);
        displayLED('f');
        break;
      }
  
      // Option 5 - System reset mode
      else if (menuOption == 5)
      {
        Serial.println("SYSTEM RESET!");
        displayLED('-');
        toggleLED(1,1,0);   // toggle orange LED

        advancedOptions = false;
        manualMode = false;

        delay(1000);
        knobCounter = 0;
        systemReset();
        break;
      }
    }
    
    // timeout
    if(current_time - last_time >= advancedOptionsTimeout)
    {
      Serial.println("advanced options timeout");
      advancedOptions = false;
      manualMode = false;

      displayLED('f');    // fade out display
      break;
    }
  }
}

// =================================================================
