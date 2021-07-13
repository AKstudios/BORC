// Interrupt Service Routines for BORC
// Updated 07/10/2021

// =================================================================
// Watchdog Interrupt Service Routine (ISR)
// =================================================================
ISR (WDT_vect)
{
  // disable watchdog timer
  wdt_disable();

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
// Rotary Encoder Interrupt Service Routine (ISR)
// =================================================================
void encoderISR()  // Interrupt Service Routine for encoder
{
  PCICR &= ~(1<<PCIE1); // disable interrupts on PCINT[15:8]
  debounceFlag = false;
  disableKnobInterrupts();
  if (knobRotateFlag == true)
  {
    current_A_state = digitalRead(CHANNEL_A);
    if (current_A_state != last_A_state  && current_A_state == 1)
    {
      if (digitalRead(CHANNEL_B) != current_A_state)
      {
        setpoint--;
        menuOption--;
        knobDirection = 1;   // knob moves anticlockwise
      }
      else
      {
        setpoint++;
        menuOption++;
        knobDirection = 2;   // knob moves clockwise
      }
      
      if (setpoint > setpointMaxValue)
      {
        setpoint = setpointMaxValue;
        displayMax = true;
      }
      else if (setpoint < setpointMinValue)
      {
        setpoint = setpointMinValue;
        displayMin = true;
      }

      if (menuOption > 6)
      {
        menuOption = 6;
      }
      else if (menuOption <= 1)
      {
        menuOption = 1;
      }

    }
    manualOverride = 1;
    last_A_state = current_A_state;
  }
  else if (knobRotateFlag == false)
  {
    knobRotateFlag = true;
  }

  if (displayFlag == false)
  {
    displayFlag = true;
  }
  
  enableKnobInterrupts();
}

// =================================================================
// Knob click Interrupt Service Routine (ISR)
// =================================================================
ISR(PCINT1_vect)  // ISR for knob click pin
{
  disableKnobInterrupts();
  debounceFlag = false;
  knobRotateFlag = true;
  if(knobClickFlag == false)
    knobClickFlag = true;

  if (displayFlag == false)
  {
    displayFlag = true;
  }
  enableKnobInterrupts();
}
