// Motor control functions for BORC
// Updated 02/12/2021

// =================================================================
// Turn servo power & servo driver power on
// =================================================================
void enableServo()
{
  Serial.println("turning servo driver on");
  pinMode(DRIVER_POWER_PIN, OUTPUT);
  pinMode(SERVO_POWER_PIN, OUTPUT);
  digitalWrite(DRIVER_POWER_PIN, HIGH);
  digitalWrite(SERVO_POWER_PIN, HIGH);
  delay(1);
  pwm.begin();
  pwm.setPWMFreq(SERVO_FREQ);
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
      servoPosition += servoIncrements;

      if (servoPosition >= servoMaxPosition)
        servoPosition = servoMaxPosition;
            
      Serial.print("Servo position: ");
      Serial.println(servoPosition);
      
      pwm.setPWM(0, 0, servoPosition);  // set servo position position
      delay(100); // give servo some time to move
    }

    else if(knobDirection == 2) // motor right (close valve)
    {
      servoPosition -= servoIncrements;

      if (servoPosition <= servoMinPosition)
        servoPosition = servoMinPosition;
       
      Serial.print("Servo position: ");
      Serial.println(servoPosition);
      
      pwm.setPWM(0, 0, servoPosition);  // set servo position position
      delay(100);   // give servo some time to move
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
}

// =================================================================
// Calibrate servo by finding min and max positions
// =================================================================
void calibrateServo()
{
  enableServo();
  
  Serial.println("calibrating...");
  
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

// =================================================================
// PID control to create new time interval
// =================================================================
//void PID()
//{
//
//}
