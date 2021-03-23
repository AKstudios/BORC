// Motor control functions for BORC
// Updated 03/23/2021

// =================================================================
// Turn servo power & servo driver power on
// =================================================================
void enableServo()
{
  // turn on servo driver and power
  controlDevices(DRIVER_POWER_PIN, HIGH);
  controlDevices(SERVO_POWER_PIN, HIGH);
  Serial.println("turning servo driver on");
  delay(1);

  // initialize servo driver
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
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
  enableServo();

  // turn motor in small increments in manual mode -----------------
  if(manualMode == true)  
  {
    Serial.println("manual true");
    if(knobDirection == 1)  // motor left (open valve)
    {
      if (SERVO_TYPE == 0)
      {
        servoPosition += servoIncrements;
  
        if (servoPosition >= servoMaxPosition)
          servoPosition = servoMaxPosition;
              
        Serial.print("Servo position: ");
        Serial.println(servoPosition);
        
        pwm.setPWM(0, 0, servoPosition);  // set servo position position
        delay(100); // give servo some time to move
      }
      else if (SERVO_TYPE == 1)
      {
        pwm.setPWM(0, 0, servoCWpulse);  // move 360 servo clockwise
        delay(1);
        currentSense();
        delay(servoTimeIncrements);   // give servo some time to move
        pwm.setPWM(0, 0, 0);  // stop 360 servo
        Serial.print("Servo current draw: ");
        Serial.println(current);
      }
    }

    else if(knobDirection == 2) // motor right (close valve)
    {
      if (SERVO_TYPE == 0)
      {
        servoPosition -= servoIncrements;

        if (servoPosition <= servoMinPosition)
          servoPosition = servoMinPosition;
         
        Serial.print("Servo position: ");
        Serial.println(servoPosition);
        
        pwm.setPWM(0, 0, servoPosition);  // set servo position position
        delay(100);   // give servo some time to move
      }
      else if (SERVO_TYPE == 1)
      {
        pwm.setPWM(0, 0, servoCCWpulse);  // move 360 servo counterclockWise
        delay(1);
        currentSense();
        delay(servoTimeIncrements);   // give servo some time to move
        pwm.setPWM(0, 0, 0);  // stop 360 servo
        Serial.print("Servo current draw: ");
        Serial.println(current);
      }
    }
  }

  // turn motor automatically --------------------------------------
  else if(manualMode == false)
  {
    Serial.println("auto");
    blinkLED(1,0,1,100);  // place holder
    // PID code goes here 
  }

//  currentSense();
//  while (current)
//  {
//    if (current < 7)  // servo stopped moving, turn off
//    {
//      Serial.println("turning servo off");
//      // turn servo off
//      digitalWrite(SERVO_POWER_PIN, LOW);
//      pinMode(SERVO_POWER_PIN, INPUT);
//      break;
//    }
//
//    else if(current >= 7) // servo still moving, wait a bit
//      delay(50);
//  }

  // once everything is done, disable servo driver and power
  disableServo();
}

// =================================================================
// Calibrate servo by finding min and max positions
// =================================================================
void calibrateServo()
{
  enableServo();
  
  Serial.println("calibrating...");
  
  if (SERVO_TYPE == 0)
  {
    // find servo max position
    for (int i = 0; i<700; i+=10)
    {
      pwm.setPWM(0, 0, i);
      currentSense();
      if (current > 7.0)
      {
        servoMaxPosition = i;
      }
      delay(250);
    }
  
    delay(500);   // wait a bit
    
    // find servo min position
    for (int i = 700; i>=0; i-=10)
    {
      pwm.setPWM(0, 0, i);
      currentSense();
      if (current > 7.0)
      {
        servoMinPosition = i;
      }
      delay(250);
    }
  
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
    Serial.println("calibrate 360 servo");
  }

  // once everything is done, disable servo driver and power
  disableServo();
}

// =================================================================
// PID control to create new time interval
// =================================================================
//void PID()
//{
//
//}
