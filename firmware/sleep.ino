// Sleep function for BORC
// Updated 09/15/2021

// =================================================================
// Sleep function
// =================================================================
void nap(char sleepMode)
{ 
  // reset counters
  menuOption = 0;

  // turn LEDs off
  toggleLED(0,0,0);

  // disable all devices to save power
  controlDevices(99, LOW);

  // put radio and flash to sleep
  radio.sleep();
  flash.sleep();
  delay(1);

  // enable knob interrupts (rotation & clicks)
  enableKnobInterrupts();

  
  
  if(sleepMode == '8')
  {
    Serial.println("WDT 8 sleep");
    delay(5);
    // set watchdog sleep
    WDTCSR = 0;
    WDTCSR = bit (WDCE) | bit (WDE);  // allow changes, disable reset
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0); // set interrupt mode and an 8 sec interval
    wdt_reset(); //pat the dog...
  }

  else if(sleepMode == '2')
  {
    Serial.println("WDT 2 sleep");
    delay(5);
    // set watchdog sleep
    WDTCSR = 0;
    WDTCSR = bit (WDCE) | bit (WDE);  // allow changes, disable reset
    WDTCSR = bit (WDIE) | bit (WDP2)| bit (WDP1) | bit (WDP0); // set interrupt mode and a 2 sec interval
    wdt_reset(); //pat the dog...
  }
  
  else if(sleepMode == 'f')
  {
    Serial.println("forever sleep");
    delay(5);
    wdt_disable();  // disable watchdog timer
  }

  // set and enable sleep mode
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);   // set power down mode
  noInterrupts(); // timed sequence follows  
  sleep_enable();
  
  // turn off brown-out detection in software before sleeping
  // BODS must be set to one and BODSE must be set to zero within four clock cycles
  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts();
  sleep_cpu();

  // device is sleeping here  --------------------------------------
  
  flash.wakeup();   // IMPORTANT - wake up flash memory so it doesn't lock out the code

  // enable essential devices after waking up due to knob or if actions need to be taken
  if(knobRotateFlag == true || knobClickFlag == true || actionsIntervalCounter == 3)
  {
    // enable all hardware devices
    controlDevices(99, HIGH);
    delay(1);

    // initialize important devices
    enableServo();
//    initializeLEDmatrix();
  }

  // time to transmit data
  else if(knobRotateFlag == false && knobClickFlag == false && transmitIntervalCounter >= (transmitInterval/8)-1)
  {
    // enable all hardware devices
    controlDevices(99, HIGH);
    delay(1);

    // initialize important devices
    enableServo();
  }
}

// =================================================================
