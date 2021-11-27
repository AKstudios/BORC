// Actions functions for BORC (system functions, control, debug)
// Updated 09/15/2021

// =================================================================
// System reset in extreme situations if code gets hanged somewhere
// =================================================================
void systemReset(bool fullReset)
{

    Serial.println("resetting");
  delay(5);
  // if full system reset requested
  if(fullReset == true)
  {
    // clear configuration settings in EEPROM
    for (int i = 0 ; i < 500 ; i++)
    {
      EEPROM.write(i, 0);
    }
  }

  // reboot system
  asm volatile ("jmp 0");
}

// =================================================================
// Control devices (turn on/off)
// =================================================================
void controlDevices(uint8_t pin, byte action)
{
  if (pin == 99)  // set all devices
  {
    if (action == 1)  // turn on devices
    {
      // set pin modes for devices as OUTPUT
      pinMode(CURRENT_SENSE_POWER_PIN, OUTPUT);
//      pinMode(SERVO_POWER_PIN, OUTPUT);
      pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT);
      pinMode(LED_SCREEN_POWER_PIN, OUTPUT);
      pinMode(DRIVER_POWER_PIN, OUTPUT);
    
      // turn on all devices
      digitalWrite(CURRENT_SENSE_POWER_PIN, HIGH);
      digitalWrite(LED_SCREEN_POWER_PIN, HIGH);
//      digitalWrite(SERVO_POWER_PIN, HIGH);
      digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);
      digitalWrite(DRIVER_POWER_PIN, HIGH);
    }
    else if (action == 0) // turn off devices
    {
      // turn off all devices
      digitalWrite(CURRENT_SENSE_POWER_PIN, LOW);
      digitalWrite(LED_SCREEN_POWER_PIN, LOW);
      digitalWrite(SERVO_POWER_PIN, LOW);
      digitalWrite(TEMP_SENSOR_POWER_PIN, LOW);
      digitalWrite(DRIVER_POWER_PIN, LOW);
      
      // set pin modes for devices as INPUT
      pinMode(DRIVER_POWER_PIN, INPUT);
      pinMode(LED_SCREEN_POWER_PIN, INPUT);
      pinMode(CURRENT_SENSE_POWER_PIN, INPUT);
      pinMode(SERVO_POWER_PIN, INPUT);
      pinMode(TEMP_SENSOR_POWER_PIN, INPUT);
    }
  }

  else  // set a specific device
  {
    if (action == 1)
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }
    else if (action == 0)
    {
      digitalWrite(pin, LOW);
      pinMode(pin, INPUT);
    }
  }
  delay(1);
}

// =================================================================
// Enable knob interrupts
// =================================================================
void enableKnobInterrupts()
{
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  
  attachInterrupt(0, encoderISR_A, RISING);
  attachInterrupt(1, encoderISR_B, RISING);

  // enable pin-change interrupt for knob clicks
  cli();  // disable global interrupts
  PORTB |= (1<<PORTB0); //Activate pullup on pin PB0
  PCIFR |= (1<<PCIF1); // clear outstanding interrupt on PCINT[15:8] 
  PCICR |= (1<<PCIE1); // enable interrupts on PCINT[15:8]
  PCMSK1 |= (1<<PCINT8); // pin change mask register for pin D0 (PCINT8)
  sei();  // enable global interrupts
}

// =================================================================
// Disable knob interrupts
// =================================================================
void disableKnobInterrupts()
{
  detachInterrupt(0);
  detachInterrupt(1);
  PCICR &= ~(1<<PCIE1); // disable interrupts on PCINT[15:8]
}

// =================================================================
// Check error code
// =================================================================
void debug()
{
  checkBattery(VBAT_SENSE);
  if(errorCode != 0 && errorCode != 0x80)  // errors detected apart from battery
    blinkLED(1,0,1,10);       // blink purple to indicate error
  else if(errorCode == 0x80)  // battery low
    blinkLED(1,0,0,10);       // blink red to indicate battery low
  else if(errorCode == 0)     // no errors
    blinkLED(0,0,0,0);        // don't blink LED
  Serial.print("Debug code: ");
  Serial.println(errorCode, HEX);
  delay(5);
}

// =================================================================
// Check battery level
// =================================================================
void checkBattery(int pin)
{
  float averageADC = readADC(pin, 5);
  batteryVoltage = (averageADC/1023) * 2 * 3.3;  // convert ADC to voltage reading
  if(batteryVoltage < 3.6)    // battery below 3.6 is low or has problem
    errorCode |= (1<<BATT_ERR);
  else if(batteryVoltage >= 3.6 && batteryVoltage <= 4.3)   // battery is nominal
    errorCode &= ~(1<<BATT_ERR);
}

// =================================================================
// Get average ADC for an analog pin with n samples
// =================================================================
float readADC(int pin, int n)
{
  float sum=0.0;
  for(int i=0;i<n;i++)
  {
     sum = sum + analogRead(pin);
  }
  float average = sum/float(n);
  return average;
}

// =================================================================
