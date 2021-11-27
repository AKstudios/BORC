// BORC firmware
// Updated 09/15/2021

// Developed by Akram Ali

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
#include <EEPROM.h>
#include <Wire.h>
#include <SPI.h>
#include "config.h"

// =================================================================
// Library objects
// =================================================================
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_DRIVER_ADDRESS);
Adafruit_IS31FL3731_Wing ledmatrix = Adafruit_IS31FL3731_Wing();
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_INA219 ina219(CURRENT_SENSE_ADDRESS);
SPIFlash flash(FLASH_SS, 0xEF30);
RFM69 radio;

// =================================================================
// Setup function
// =================================================================
void setup()
{
  // disable watchdog timer just in case
  MCUSR = 0;
  wdt_disable();  

  // turn LEDs off
  toggleLED(0,0,0);

  // disable all devices on power up
  controlDevices(99, LOW);

  // Initialize Serial port
  Serial.begin(SERIAL_BAUD);
  Serial.println("Power on");

  // Fade blue LED to indicate a power up
  fadeLED(BLUE);    

  // Set configFlag to true for testing
  configFlag = true;
  
//  // check device configuration status
//  EEPROM.get(CONFIGFLAGADDRESS, configFlag);
//  if (configFlag == true) // if device not configured, use default values
//  {
//    Serial.println("Device configured, using EEPROM variables to init radio");
//    getNodeParams(); // this will update default variables using data stored in EEPROM
//  }
//  else if (configFlag == false)
//  {
//    Serial.println("Device not configured, using default variables to init radio");
//  }
  
  Serial.println(NODEID);
  Serial.println(NETWORKID);
  Serial.println(ENCRYPTKEY);

  // initialize radio
  initializeRadio();
  
  // initialize flash
  if(!flash.initialize())
    errorCode |= (1<<FLASH_ERR);
  else
    errorCode &= ~(1<<FLASH_ERR);
  
  // get UID from flash
  uint8_t* MACID = flash.readUniqueId();  // get unique 64-bit ID from flash
  for (int i=0; i<8; i++)
    _MACID +=  String(MACID[i], HEX); // convert uint8_t* to HEX String
  _MACID.toCharArray(UID, _MACID.length()+1); // convert String to char array to use in datapackets
  Serial.println(UID);

  // enable all hardware devices
  controlDevices(99, HIGH);
  delay(10);
  
  // enable servo driver and set servo to min
  enableServo();
  if (SERVO_TYPE == 0)  // 270 servo
    pwm.setPWM(0, 0, servoMinPosition); // start from min position for 270 servo
  
  // initialize LED matrix
  initializeLEDmatrix();

  // enable current sensor
  currentSense();

  // enable temp/RH sensor
  readTempRH();

  // Read the initial state of A
//  last_A_state = digitalRead(CHANNEL_A);

  // setup interrupts
  enableKnobInterrupts();

  // get initial time
  last_time = millis();
  
  Serial.println("Ready");
  
  delay(100); //wait a bit till everything settles in
}

// =================================================================
// Main loop
// =================================================================
void loop()
{
  // device not configured -----------------------------------------
  if (configFlag == false)
  {
    // go back to config mode if knob clicked once
    configurationMode();

    // if device still not configured
    if (configFlag == false)
    {
      // sleep forever
      nap('f');
    }

    // if knob was clicked during configuration mode
    if(knobRotateFlag == true && knobClickFlag == true && configFlag == false)
    {
      // keep track of last time for timeout
      last_time = millis();

      // if the button still pressed
      while(digitalRead(KNOB_CLICK) == 0)
      {
        // keep track of current time
        current_time = millis();
        
        // enable demo mode when pressed and held for ~1.5 - 4 secs
        if(current_time - last_time >= 1500 && current_time - last_time <= 4000)
        {
          Serial.println("now in demo mode");

          // temporarily set configFlag to true. Actual value still in EEPROM, resets on reboot
          configFlag = true;

          // toggle white LED to indicate temporary demo mode active
          toggleLED(1,1,1);

          // give some delay so LED is visible for a bit
          delay(1000);

          // turn LEDs off
          toggleLED(0,0,0);

          // reset knob click flag
          knobClickFlag = false;
  
          // reset knob rotate flag
          knobRotateFlag = false;

          // break out of while loop
          break;
        }

        // if knob pressed for more than the knob timeout time
        if(current_time - last_time > knobTimeout)
        {
          Serial.println("timeout of button press");

          // break out of while loop
          break;
        }

        // if knob is no longer being held
        if(digitalRead(KNOB_CLICK) == 1)
        {
          // break out of loop
          break;
        }
      }
    }

    // give some delay
    delay(1);
  }

  // device already configured -------------------------------------
  else if(configFlag == true)
  {
    Serial.println("config true");
    // keep the last millis time in memory
    last_time = millis();

    // reset watchdog timer
    wdt_reset();
  
    // Go to sleep if knob not interacted with ---------------------------------------------------
    if(knobRotateFlag == false && knobClickFlag == false && WDTflag == false)
    {
      
      Serial.println("watchdog sleep");
      delay(5);

      // save debug info
//      debug();

      // save previous display mode
      _displayMode = displayMode;

      // go to sleep in regular watchdog mode (8 seconds sleep)
      nap('8');

//      // check whether regular or quick WDT sleep is needed
//      if(quickWDT == false)
//      {
//        // go to sleep in regular watchdog mode (8 seconds sleep)
//        nap('8');
//      }
//
//      else if(quickWDT = true)
//      {
//        // go to sleep in quick watchdog mode (2 seconds sleep)
//        nap('2');
//
//        // reset quick WDT flag to false
//        quickWDT = false;
//      }
    }
  
    // WDT triggered, knob untouched ---------------------------------
    else if(knobRotateFlag == false && knobClickFlag == false && WDTflag == true)
    {
      // check interval counters
      checkCounters();

      // if enough time has passed, do actions (3 intervals = ~32 seconds),
      if(actionsFlag == true)
      {
        // read temperature and humidity
//        readTempRH();
        Serial.print("Temp: ");
        Serial.println(temp);
        Serial.print("RH: ");
        Serial.println(rh);
        
        // do servo control here


        // reset actions flag to false
        actionsFlag = false;
      }

      // if enough time has passed, transmit data (every ~56 secs)
      if(transmitFlag == true)
      {
        Serial.println("sending data");

        // create a data send packet
        createDataPacket('s');

        // send data; true argument waits for ACK to come back
        sendData(true);

        // reset transmit flag to false
        transmitFlag = false;
      }

      // reset watchdog flag back to false
      WDTflag = false;
    }
  
    // knob interacted with ------------------------------------------
    else if((knobRotateFlag == true || knobClickFlag == true) && WDTflag == false)
    {
      // check knob's current status and set what to display
      checkKnobStatus();

      // reset knob click flag
      knobClickFlag = false;

      // reset knob rotate flag
      knobRotateFlag = false;

      // check if the display flag is set
      if(displayFlag == true)
      {
        Serial.println("update display");
        // update display
        updateDisplay(displayMode);

        // reset display flag
        displayFlag = false;
      }

      // keep the last millis time in memory
      last_time = millis();

      // stay in while loop for a bit
      while(1)
      {
        Serial.println("while 1");
        
        // keep track of current time
        current_time = millis();

        // if enough time has passed
        if(current_time - last_time >= displayTimeout)
        {
          Serial.println("timeout");
          // reset advanced options flag
          advancedOptions = false;

          // reset knob click flag
          knobClickFlag = false;
  
          // reset knob rotate flag
          knobRotateFlag = false;

          // fade out display
          updateDisplay('f');

          // break out of while loop
          break;
        }

        // if knob interacted with during the while loop
        if(knobRotateFlag == true || knobClickFlag == true)
        {
          Serial.println("knob interacted with, break out of while");
          // break out of loop
          break;
        }
      }
    }
  }
}

// =================================================================
// checks and updates interval counters and flags
// =================================================================
void checkCounters()
{
  Serial.print("actions counter: ");
  Serial.println(actionsIntervalCounter);
  Serial.print("transmit counter: ");
  Serial.println(transmitIntervalCounter);
      
  // if enough time has passed for actions (3 intervals = ~32 seconds),
  if(actionsIntervalCounter == 3)
  {
    // set action flag to true
    actionsFlag = true;

    // reset actions interval counter to 0
    actionsIntervalCounter = 0;
  }

  // not enough time has passed yet for taking actions
  else if(actionsIntervalCounter < 3)
  {
    // ensure action flag is still false
    actionsFlag = false;
    
    // increment actions interval counter
    actionsIntervalCounter++;
  }

  // if enough time has passed for data transmission (every ~56 secs)
  if(transmitIntervalCounter == (transmitInterval/8)-1)
  {
    // set transmit flag to true
    transmitFlag = true;
    
    // reset transmit interval counter to 0
    transmitIntervalCounter = 0;
  }

  // not enough time has passed yet for sending data
  else if(transmitIntervalCounter < (transmitInterval/8)-1)
  {
    // ensure transmit flag is still false
    transmitFlag = false;
    
    // increment transmit interval counter
    transmitIntervalCounter++;
  }

  
}

// =================================================================
