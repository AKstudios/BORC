// Interrupt Service Routines for BORC
// Updated 09/15/2021

// =================================================================
// Watchdog Interrupt Service Routine (ISR)
// =================================================================
ISR (WDT_vect)
{
  // disable watchdog timer
//  wdt_disable();

  // enable watchdog flag
  if(WDTflag == false)
    WDTflag = true;

  // if watchdog timer triggered by system reset flag
  if(servoWDTflag == true)
  {
    // reboot system
    systemReset(false);
  }
}

// =================================================================
// Rotary Encoder Interrupt Service Routine (ISR) for channel A
// =================================================================
void encoderISR_A()  // Interrupt Service Routine
{
  // handle encoder only after debounce is handled
  static unsigned long lastInterrupt = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterrupt > rotateDebounceInterval)
  {
    // read current state of both encoder pins
    aState = digitalRead(CHANNEL_A);
    bState = digitalRead(CHANNEL_B);
  
    // if both pins at high detent and coming previously from a moving position
    if (aState == HIGH && bState == HIGH && bFlag == true) 
    {
      // decrements
      setpoint--;
      menuOption--;
      knobDirection = 1;   // knob moves anticlockwise

      // prevent overflow
      if(setpoint < setpointMinValue)
      {
        setpoint = setpointMinValue;
        displayMin = true;
      }
      
      if (menuOption <= 1)
        menuOption = 1;

      // set knob rotate flag to true
      if (knobRotateFlag == false)
        knobRotateFlag = true;
  
      // set display flag to true
      if (displayFlag == false)
        displayFlag = true;
        
      // reset encoder flags
      aFlag = false;
      bFlag = false;
    }
  
    // encoder is currently moving
    else if(aState == LOW && bState == HIGH)
      aFlag = true;   // logic adapted from https://www.instructables.com/Improved-Arduino-Rotary-Encoder-Reading/
  
    // set manual override flag
    manualOverride = 1;
  }
  
  // reset last interrupt time
  lastInterrupt = interruptTime;
}

// =================================================================
// Rotary Encoder Interrupt Service Routine (ISR) for channel B
// =================================================================
void encoderISR_B()  // Interrupt Service Routine
{
  // handle encoder only after debounce is handled
  static unsigned long lastInterrupt = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterrupt > rotateDebounceInterval)   // debounce logic based on https://www.circuitxcode.com/debounce-interrupt-handler/
  {
    // read current state of both encoder pins
    aState = digitalRead(CHANNEL_A);
    bState = digitalRead(CHANNEL_B);
  
    // if both pins at high detent and coming previously from a moving position
    if (aState == HIGH && bState == HIGH && aFlag == true) 
    {
      // increments
      setpoint++;
      menuOption++;
      knobDirection = 2;   // knob moves anticlockwise
  
      // prevent overflow
      if (setpoint > setpointMaxValue)
      {
        setpoint = setpointMaxValue;
        displayMax = true;
      }
  
      if (menuOption > 6)
        menuOption = 6;

      // set knob rotate flag to true
      if (knobRotateFlag == false)
        knobRotateFlag = true;
  
      // set display flag to true
      if (displayFlag == false)
        displayFlag = true;
      
      // reset encoder flags
      aFlag = false;
      bFlag = false;
    }
  
    // encoder is currently moving
    else if(aState == HIGH && bState == LOW)
      bFlag = true;
  
    // set manual override flag
    manualOverride = 1;
  }
  
  // reset last interrupt time
  lastInterrupt = interruptTime;
}
// =================================================================
// Knob click Interrupt Service Routine (ISR)
// =================================================================
ISR(PCINT1_vect)  // ISR for knob click pin
{
  // handle encoder only after debounce is handled
  static unsigned long lastInterrupt = 0;
  unsigned long interruptTime = millis();
  if (interruptTime - lastInterrupt > clickDebounceInterval)
  {
    if(digitalRead(KNOB_CLICK) == 0) // only if knob is depressed
    {
      if(knobClickFlag == false)
        knobClickFlag = true;
    
      if (displayFlag == false)
        displayFlag = true;
    }
  }
  // reset last interrupt time
  lastInterrupt = interruptTime;
}
