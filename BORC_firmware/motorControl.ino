// Motor control functions for BORC
// Updated 12/22/2020

// =================================================================
// Run motor for a specified time interval and in a given direction
// =================================================================
void runMotor()
{
  Serial.println("turning motor on");
  // enable motor power
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_CONTROL_PIN, OUTPUT);
  digitalWrite(MOTOR_CONTROL_PIN, HIGH);

  // turn motor in small increments in manual mode
  if(manualMode == true)  
  { 
    Serial.println("manual true");
    if(knobDirection == 1)  // motor left
    {
      Serial.println("motor left");
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, HIGH);
      delay(50);
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
    }

    else if(knobDirection == 2) // motor right
    {
      Serial.println("motor right");
      digitalWrite(MOTOR_IN1, HIGH);
      digitalWrite(MOTOR_IN2, LOW);
      delay(50);
      digitalWrite(MOTOR_IN1, LOW);
      digitalWrite(MOTOR_IN2, LOW);
    }
  }

  // turn motor automatically
  else if(manualMode == false)
  {
    Serial.println("auto");
    blinkLED(1,0,1,100);  // place holder
    // PID code goes here 
  }

  Serial.println("turning motor off");
  // turn motor off
  digitalWrite(MOTOR_CONTROL_PIN, LOW);
  pinMode(MOTOR_CONTROL_PIN, INPUT);
  pinMode(MOTOR_IN1, INPUT);
  pinMode(MOTOR_IN2, INPUT);
}

// =================================================================
// PID control to create new time interval
// =================================================================
//void PID()
//{
//
//}
