// Knob UX functions for BORC
// Updated 01/20/2021

// =================================================================
// Function to check knob status
// =================================================================
void checkKnobStatus()
{
  // Knob rotated --------------------------------------------------
  if(knobFlag == true && knobClickFlag == false && WDTflag == false && knobDirection != 0)
  {
    if(manualMode == false)
    {
      displayLED('s');
      toggleLED(0,0,0); // turn LEDs off
      knobDirection = 0;    // reset knob direction
    }

    else if(manualMode == true)
    {
      displayLED('<');
      toggleLED(0,0,0); // turn LEDs off
      runMotor();
      knobDirection = 0;    // reset knob direction
    }
    last_time = millis();
  }

  // Knob clicked --------------------------------------------------
  else if(knobFlag == true && knobClickFlag == true && WDTflag == false && knobDirection == 0)
  {
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

      // display current temp when knob held for 1-4 secs
      if(current_time - last_time >= 1500 && current_time - last_time <= 4500)
      {
        toggleLED(1,1,1); // toggle white LED
        displayLED('t');
      }

      // display manual/calibration menu when knob held for 4-7 secs
      else if(current_time - last_time >= 4500 && current_time - last_time <= 7500)
      {
        toggleLED(0,1,1); // toggle teal LED
        if(advancedOptions == false)
        {
          Serial.println("toggle advanced options");
          advancedOptions = true;
          if(manualMode == false)
            displayLED('M');
          else if(manualMode == true)
            displayLED('S');
          delay(1000);
          break;
        }
      }

      // if button held more than 7 seconds, timeout
      else if(current_time - last_time > 7500)
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
      displayLED('f');
      knobFlag = false;
    }
  }
  
  // Advanced options menu -----------------------------------------
  while(advancedOptions == true)
  {
    current_time = millis();
    
    // manual mode
    if (knobDirection == 0 || knobDirection == 1)
    {
      if(manualMode == false)
      {
        displayLED('M');
        if(digitalRead(KNOB_CLICK) == 0)
        {
          Serial.println("m set");
          advancedOptions = false;
          manualMode = true;
          calibrationMode = false;
          delay(500);
          displayLED('<');
          toggleLED(0,0,0);
          break;
        }
      }

      // setpoint mode
      else if(manualMode == true)
      {
        displayLED('S');
        if(digitalRead(KNOB_CLICK) == 0)
        {
          Serial.println("s set");
          advancedOptions = false;
          manualMode = false;
          calibrationMode = false;
          delay(500);
          setpoint = 72;  // reset setpoint to 72
          displayLED('s');
          toggleLED(0,0,0);
          break;
        }
      }
    }
    
    // calibration mode
    else if (knobDirection == 2)
    {
      displayLED('C');
      if(digitalRead(KNOB_CLICK) == 0)
      {
        Serial.println("c set");
        advancedOptions = false;
        calibrationMode = true;
        manualMode = false;
        delay(500);
        displayLED('x');
        toggleLED(0,0,0);
        break;
      }
    }

    // timeout
    if(current_time - last_time >= 7000)
    {
      Serial.println("advanced options timeout");
      calibrationMode = false;
      manualMode = false;
      advancedOptions = false;
      knobFlag = false;
      displayLED('f');    // fade out display
      break;
    }
  }
}

// =================================================================
