// Sleep function for BORC
// Updated 04/07/2021

// =================================================================
// Sleep function
// =================================================================
void sleep(char sleepMode)
{ 
  // reset counters
  knobCounter = 0;
  menuOption = 0;  

  // turn LEDs off
  toggleLED(0,0,0);

  // disable all devices to save power
  controlDevices(99, LOW);

  // put radio and flash to sleep
  radio.sleep();
  flash.sleep();
  delay(1);

  // enable knob rotation interrupts
  enableKnobInterrupts();
  
  // enable pin-change interrupt for knob clicks
  cli();  // disable global interrupts
  PORTB |= (1<<PORTB0); //Activate pullup on pin PB0
  PCIFR |= (1<<PCIF1); // clear outstanding interrupt on PCINT[15:8] 
  PCICR |= (1<<PCIE1); // enable interrupts on PCINT[15:8]
  PCMSK1 |= (1<<PCINT8); // pin change mask register for pin D0 (PCINT8)
  sei();  // enable global interrupts

  // disable internal stuff to save power
  ADCSRA = 0;  // disable ADC
  MCUSR = 0;    // clear various "reset" flags
  
  if(sleepMode == 'w')
  {
    Serial.println("WDT sleep");
    delay(5);
    // set watchdog sleep
    WDTCSR = bit (WDCE) | bit (WDE);  // allow changes, disable reset
    WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0); //set interrupt mode and an 8 sec interval
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
  
  sleep_disable();  //cancel sleep as a precaution
  flash.wakeup();   // IMPORTANT - wake up flash memory so it doesn't lock out the code

  // enable essential devices after waking up due to knob or if actions need to be taken
  if(knobFlag == true || knobClickFlag == true || actionsIntervalCounter == 3)
  {
    // enable all hardware devices
    controlDevices(99, HIGH);
    delay(1);

    // initialize important devices
    enableServo();
//    initializeLEDmatrix();
  }

  // time to transmit data
  else if(knobFlag == false && knobClickFlag == false && transmitIntervalCounter >= (transmitInterval/8)-1)
  {
    // enable all hardware devices
    controlDevices(99, HIGH);
    delay(1);

    // initialize important devices
    enableServo();
  }
}

// =================================================================
