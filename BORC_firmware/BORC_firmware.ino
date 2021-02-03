

// BORC firmware v2021.01.21.1
// Updated 01/20/2021

// Developed by AKstudios

// =================================================================
// Libraries
// =================================================================
#include <RFM69.h>                    //https://www.github.com/lowpowerlab/rfm69
#include <SPIFlash.h>                 //https://github.com/LowPowerLab/SPIFlash          
#include <Adafruit_GFX.h>             //https://github.com/adafruit/Adafruit-GFX-Library
#include <Adafruit_SHT31.h>           //https://github.com/adafruit/Adafruit_SHT31
#include <Adafruit_INA219.h>          //https://github.com/adafruit/Adafruit_INA219
#include <Adafruit_IS31FL3731.h>      //https://github.com/adafruit/Adafruit_IS31FL3731
#include <Adafruit_PWMServoDriver.h>  //https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <Wire.h>
#include <SPI.h>
#include "config.h"

// =================================================================
// Library objects
// =================================================================
Adafruit_IS31FL3731_Wing ledmatrix = Adafruit_IS31FL3731_Wing();
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_INA219 ina219;
SPIFlash flash(FLASH_SS, 0xEF30);
RFM69 radio;

// =================================================================
// Watchdog Interrupt Service Routine (ISR)
// =================================================================
ISR (WDT_vect)
{
  wdt_disable();  // disable watchdog timer
  if(WDTflag == false)
    WDTflag = true;
}

// =================================================================
// Rotary Encoder Interrupt Service Routine (ISR)
// =================================================================
void encoderISR()  // Interrupt Service Routine for encoder
{
  PCICR &= ~(1<<PCIE1); // disable interrupts on PCINT[15:8]
  knobCounter++;
  detachInterrupt(0);
  detachInterrupt(1);
  if (knobFlag == true)
  {
    current_A_state = digitalRead(CHANNEL_A);
    if (current_A_state != last_A_state  && current_A_state == 1)
    {
      if (digitalRead(CHANNEL_B) != current_A_state)
      {
        setpoint--;
        knobDirection = 1;   // knob moves anticlockwise
      }
      else
      {
        setpoint++;
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
    }
    last_A_state = current_A_state;
  }
  else if (knobFlag == false)
  {
    knobFlag = true;
  }
  attachInterrupt(0, encoderISR, CHANGE);  // enable hardware interrupt
  attachInterrupt(1, encoderISR, CHANGE);  // enable hardware interrupt
}

// =================================================================
// Knob click Interrupt Service Routine (ISR)
// =================================================================
ISR(PCINT1_vect)  // ISR for knob click pin
{
  knobCounter = 10;
  knobFlag = true;
  PCICR &= ~(1<<PCIE1); // disable interrupts on PCINT[15:8]
  if(knobClickFlag == false)
    knobClickFlag = true;
}

// =================================================================
// Setup function
// =================================================================
void setup()
{
  Serial.begin(SERIAL_BAUD);
  Serial.println("Power on");
  fadeLED(BLUE);    // this indicates a power up
  
  // set pin modes
  pinMode(CURRENT_SENSE_POWER_PIN, OUTPUT);
  pinMode(SERVO_POWER_PIN, OUTPUT);
  pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT);
  pinMode(LED_SCREEN_POWER_PIN, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
//  pinMode(A, INPUT);
//  pinMode(B, INPUT);

  // turn on all devices
  digitalWrite(CURRENT_SENSE_POWER_PIN, HIGH);
  digitalWrite(LED_SCREEN_POWER_PIN, HIGH);
  digitalWrite(SERVO_POWER_PIN, HIGH);
  digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);

  // Read the initial state of A
  last_A_state = digitalRead(CHANNEL_A);  

  // setup interrupts
  attachInterrupt(0, encoderISR, CHANGE);
  attachInterrupt(1, encoderISR, CHANGE);

  // initialize radio
  if(!radio.initialize(FREQUENCY,NODEID,NETWORKID))
    errorCode |= (1<<RADIO_ERR);
  else
    errorCode &= ~(1<<RADIO_ERR);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
  radio.encrypt(ENCRYPTKEY);
  radio.sleep();

  // initialize flash
  if(!flash.initialize())
    errorCode |= (1<<FLASH_ERR);
  else
    errorCode &= ~(1<<FLASH_ERR);
  flash.sleep();

  // setup LED matrix
  if(!ledmatrix.begin())
    errorCode |= (1<<LED_DRV_ERR);
  else
    errorCode &= ~(1<<LED_DRV_ERR);

  // enable current sensor
  currentSense();

  // enable temp/RH sensor
  readTempRH();

  // get initial time
  last_time = millis();

  Serial.println("Ready");
}

// =================================================================
// Main loop
// =================================================================
void loop()
{
  last_time = millis(); // keep the last millis time in memory
  wdt_reset();

  // Go to sleep ---------------------------------------------------
  if(knobFlag == false && knobClickFlag == false && WDTflag == false && knobDirection == 0)
  {
    last_time = millis(); // save last millis time
    Serial.println("00 sleep");
    delay(5);
    debug();
    sleep(); // sleep every loop
  }

  // WDT triggered, knob untouched ---------------------------------
  else if(knobFlag == false && knobClickFlag == false && WDTflag == true && knobDirection == 0)
  {
    if(actionsIntervalCounter == 3) // if enough time has passed, do actions (3 intervals = ~32 seconds),
    {
      Serial.print("actions counter: ");
      Serial.println(actionsIntervalCounter);
      pinMode(SERVO_POWER_PIN, OUTPUT);
      digitalWrite(SERVO_POWER_PIN, HIGH);
      readTempRH();
      currentSense();
      Serial.print("Temp: ");
      Serial.println(temp);
      Serial.print("RH: ");
      Serial.println(rh);
      Serial.print("Bus voltage: ");
      Serial.println(busVoltage);
      Serial.print("current: ");
      Serial.println(motorCurrent);
      actionsIntervalCounter = 0; // reset watchdog counter to 0
    }
    else if(actionsIntervalCounter < 3)
    {
      Serial.print("actions counter: ");
      Serial.println(actionsIntervalCounter);
      actionsIntervalCounter++;
    }
    
    if(transmitIntervalCounter == 6) // send data every ~54 secs
    {
      Serial.println("sending data, resetting to 0");
      transmitIntervalCounter = 0;
    }
    else if(transmitIntervalCounter < 6)
    {
      Serial.print("transmit counter: ");
      Serial.println(transmitIntervalCounter);
      transmitIntervalCounter++;
    }
    last_time = millis(); // save last millis time
    WDTflag = false;
  }

  // knob interacted with ------------------------------------------
  else
    checkKnobStatus();
}

// =================================================================
// Blink LED
// =================================================================
void blinkLED(byte R, byte G, byte B, int DELAY_MS)
{
  // set pin modes to output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // write requested LED value
  digitalWrite(RED,R);
  digitalWrite(GREEN,G);
  digitalWrite(BLUE,B);

  // delay requested time
  delay(DELAY_MS);

  // turn LEDs off
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  // set pin modes back to input
  pinMode(RED, INPUT);
  pinMode(GREEN, INPUT);
  pinMode(BLUE, INPUT);
}

// =================================================================
// Fade LED
// =================================================================
void fadeLED(int pin)
{
  int brightness = 0;
  int fadeAmount = 5;
  for(int i=0; i<510; i=i+5)  // 255 is max analog value, 255 * 2 = 510
  {
    analogWrite(pin, brightness);  // pin 9 is LED
  
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;  // increment brightness level by 5 each time (0 is lowest, 255 is highest)
  
    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= 0 || brightness >= 255)
    {
      fadeAmount = -fadeAmount;
    }
    // wait for 20-30 milliseconds to see the dimming effect
    delay(10);
  }
  digitalWrite(pin, LOW); // switch LED off at the end of fade
}

// =================================================================
// Toggle LEDs manually
// =================================================================
void toggleLED(byte R, byte G, byte B)
{
  if (R==0 && G==0 && B==0) // set pin modes and turn off LEDs
  {
    // turn LEDs off
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);

    // set pin modes back to input
    pinMode(RED, INPUT);
    pinMode(GREEN, INPUT);
    pinMode(BLUE, INPUT);
  }

  else  // set pin modes and toggle LEDs
  {
    // set pin modes to output
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
  
    // write requested LED value
    digitalWrite(RED,R);
    digitalWrite(GREEN,G);
    digitalWrite(BLUE,B);
  }
}

// =================================================================
