// Motor control functions for BORC
// Updated 04/19/2021

// =================================================================
// Turn servo power & servo driver power on
// =================================================================
void enableServo()
{
  // turn on servo driver
  controlDevices(DRIVER_POWER_PIN, HIGH);
//  controlDevices(SERVO_POWER_PIN, HIGH);
  Serial.println("turning servo driver on");
  delay(1);

  // initialize servo driver
//  pwm.begin();
//  pwm.setPWMFreq(SERVO_FREQ);

  Serial.println("servo driver initialized");
  delay(1);
}

// =================================================================
// Turn servo power & servo driver power off
// =================================================================
void disableServo()
{
  // turn off servo driver and power
  controlDevices(DRIVER_POWER_PIN, LOW);
  controlDevices(SERVO_POWER_PIN, LOW);
  Serial.println("turning servo driver off");
  delay(1);
}

// =================================================================
// Run motor for a specified time interval and in a given direction
// =================================================================
void runServo()
{
  // enable WDT so device resets in case code hangs
  MCUSR = 0;    // clear various "reset" flags
  WDTCSR = bit (WDCE) | bit (WDE);  // allow changes, disable reset
  WDTCSR = bit (WDIE) | bit (WDP2) | bit (WDP1); //set interrupt mode and an 1 sec interval
  wdt_reset(); //pat the dog...
  servoWDTflag = true;

  Serial.println("turning servo power on");
  // turn on servo power
  controlDevices(SERVO_POWER_PIN, HIGH);
  delay(100);

  // turn motor in small increments in manual mode -----------------
  if(manualMode == true)  
  {
    Serial.println("manual true");
    if(knobDirection == 1)  // motor left (open valve)
    {
      toggleLED(1,0,0);  // red color to indicate heat
      if (SERVO_TYPE == 0) // 270 servo
      {
        servoPosition += servoIncrements;
  
        if (servoPosition >= servoMaxPosition)
          servoPosition = servoMaxPosition;
              
        Serial.print("Servo position: ");
        Serial.println(servoPosition);

        disableKnobInterrupts();
        pwm.setPWM(0, 0, servoPosition);  // set servo position position
        delay(350);
        
//        float avgCurrent = avgCurrentSense();
//        while (avgCurrent >= 7.0)
//        {
//          Serial.print("Current draw: ");
//          Serial.println(avgCurrent);
//          delay(1);
//          avgCurrent = avgCurrentSense();
//          if (avgCurrent < 7.0)
//            break;
//        }
//        delay(300); // give servo some time to move
        enableKnobInterrupts();
      }
      else if (SERVO_TYPE == 1) // 360 servo
      {
        Serial.println("moving servo CW");
        disableKnobInterrupts();
        pwm.setPWM(0, 0, servoCWpulse);  // move 360 servo clockwise
        currentSense();
        delay(servoTimeIncrements);   // give servo some time to move
        pwm.setPWM(0, 0, 4096);  // stop 360 servo
        enableKnobInterrupts();
        Serial.print("Servo current draw CW: ");
        Serial.println(current);
      }
      toggleLED(0,0,0);
    }

    else if(knobDirection == 2) // motor right (close valve)
    {
      toggleLED(0,0,1);  // blue color to indicate cool
      if (SERVO_TYPE == 0) // 270 servo
      {
        servoPosition -= servoIncrements;

        if (servoPosition <= servoMinPosition)
          servoPosition = servoMinPosition;
         
        Serial.print("Servo position: ");
        Serial.println(servoPosition);

        disableKnobInterrupts();
        pwm.setPWM(0, 0, servoPosition);  // set servo position position
        delay(350);
        
//        float avgCurrent = avgCurrentSense();
//        while (avgCurrent >= 7.0)
//        {
//          Serial.print("Current draw: ");
//          Serial.println(avgCurrent);
//          delay(1);
//          avgCurrent = avgCurrentSense();
//          if (avgCurrent < 7.0)
//            break;
//        }
//        delay(300); // give servo some time to move
        enableKnobInterrupts();
      }
      else if (SERVO_TYPE == 1) // 360 servo
      {
        Serial.println("moving servo CCW");
        disableKnobInterrupts();
        pwm.setPWM(0, 0, servoCCWpulse);  // move 360 servo counterclockWise
        currentSense();
        delay(servoTimeIncrements);   // give servo some time to move
        pwm.setPWM(0, 0, 4096);  // stop 360 servo
        enableKnobInterrupts();
        Serial.print("Servo current draw CCW: ");
        Serial.println(current);
      }
    }
    toggleLED(0,0,0); // turn LEDs off
  }

  // turn motor automatically --------------------------------------
  else if(manualMode == false)
  {
    Serial.println("auto mode - WIP");
    blinkLED(1,0,1,100);  // place holder
    // PID code goes here 
  }

  // once everything is done, turn off servo power
  Serial.println("turning servo power off");
  controlDevices(SERVO_POWER_PIN, LOW);

  wdt_reset(); //pat the dog...
  wdt_disable(); // disable WDT
  servoWDTflag = false;
}

// =================================================================
// Calibrate servo by finding min and max positions
// =================================================================
void calibrateServo()
{
  // diable interrupts
  
          // toggle Red LED to indicate calibration active
        toggleLED(1,0,0);
  
  //  enableServo();
  
  // turn on servo power
  controlDevices(SERVO_POWER_PIN, HIGH);
  
  Serial.println("calibrating 270 servo...");
  
  if (SERVO_TYPE == 0)
  {
    // find servo max position
    for (int i = 0; i<=700; i+=servoIncrements)
    {
      pwm.setPWM(0, 0, i);
      float avgCurrent = avgCurrentSense();
      Serial.print(i);
      Serial.print(" ");
      Serial.println(avgCurrent);
      if (avgCurrent > 7.0)   // servo idle current draw ~4.5mA
      {
        servoMaxPosition = i;
      }
      delay(300);
    }
  
    delay(500);   // wait a bit
    
    // find servo min position
    for (int i = 700; i>=0; i-=servoIncrements)
    {
      pwm.setPWM(0, 0, i);
      float avgCurrent = avgCurrentSense();
      Serial.print(i);
      Serial.print(" ");
      Serial.println(avgCurrent);
      if (avgCurrent > 7.0)  // servo idle current draw ~4.5mA
      {
        servoMinPosition = i;
      }
      delay(300);
    }

    // set the servo position to the min position to resume movement from that point
    servoPosition = servoMinPosition;
  
    Serial.println("=======================");
    Serial.print("Servo max position: ");
    Serial.println(servoMaxPosition);
    Serial.print("Servo min position: ");
    Serial.println(servoMinPosition);
    Serial.println("=======================");
    delay(5);
  }

  else if (SERVO_TYPE == 1)
  {
    Serial.println("calibration of 360 servo not available right now.");
  }

  // once everything is done, turn off servo power
  controlDevices(SERVO_POWER_PIN, LOW);

          // toggle Green LED to indicate calibration complete
        toggleLED(1,0,0);

        // enable interrupts
}

// =================================================================
// PID control to create new time interval
// =================================================================
//void PID()
//{
//
//}
