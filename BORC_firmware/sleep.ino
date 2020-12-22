// Sleep function for BORC
// Updated 12/22/2020

// =================================================================
// Sleep function
// =================================================================
void sleep()
{ 
  // reset knob counter
  knobCounter = 0;

  // turn LEDs off
  toggleLED(0,0,0);
  
  // turn off all devices to be sure
  digitalWrite(CURRENT_SENSE_POWER_PIN, LOW);
  digitalWrite(LED_SCREEN_POWER_PIN, LOW);
  digitalWrite(MOTOR_CONTROL_PIN, LOW);
  digitalWrite(TEMP_SENSOR_POWER_PIN, LOW);
  pinMode(LED_SCREEN_POWER_PIN, INPUT);
  pinMode(CURRENT_SENSE_POWER_PIN, INPUT);
  pinMode(MOTOR_CONTROL_PIN, INPUT);
  pinMode(TEMP_SENSOR_POWER_PIN, INPUT);
  pinMode(MOTOR_IN1, INPUT);
  pinMode(MOTOR_IN2, INPUT);
  radio.sleep();
  flash.sleep();
  delay(1);

  // enable pin-change interrupt for knob clicks
  cli();  // disable global interrupts
  PORTB |= (1<<PORTB0); //Activate pullup on pin PB0
  PCIFR |= (1<<PCIF1); // clear outstanding interrupt on PCINT[15:8] 
  PCICR |= (1<<PCIE1); // enable interrupts on PCINT[15:8]
  PCMSK1 |= (1<<PCINT8); // pin change mask register for pin D0 (PCINT8)
  sei();  // enable global interrupts

  // set watchdog sleep
  ADCSRA = 0;  // disable ADC
  MCUSR = 0;    // clear various "reset" flags
  WDTCSR = bit (WDCE) | bit (WDE);  // allow changes, disable reset
  WDTCSR = bit (WDIE) | bit (WDP3) | bit (WDP0); //set interrupt mode and an 8 sec interval
  wdt_reset(); //pat the dog...

  // enable sleep mode
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);   // set power down mode
  noInterrupts(); // timed sequence follows  
  sleep_enable();
  
  // turn off brown-out enable in software
  // BODS must be set to one and BODSE must be set to zero within four clock cycles
  // The BODS bit is automatically cleared after three clock cycles
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts();
  sleep_cpu();

  // device is sleeping here  --------------------------------------
  
  sleep_disable();  //cancel sleep as a precaution

  // only enable essential devices when needed
  if(knobFlag == true || knobClickFlag == true || actionsIntervalCounter == 3)
  {
    pinMode(CURRENT_SENSE_POWER_PIN, OUTPUT);
    pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT); 
    digitalWrite(CURRENT_SENSE_POWER_PIN, HIGH);
    digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);
    delay(1);
  }
}

// =================================================================
