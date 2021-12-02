//=========================================================================================================
// CServoDriver() - A class that manages the servo driver NXP PCA9685               
//=========================================================================================================
#include "servo.h"
#include "common.h"
#include "Arduino.h"
#include "globals.h"
#include <Adafruit_PWMServoDriver.h>  //https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

// initialize all library objects
static Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_DRIVER_ADDRESS);

// define servo's PWM frequency. Depends on type of servo
#define SERVO_FREQ  50

//=========================================================================================================
// init() - initialize the servo driver
//=========================================================================================================
void CServoDriver::init()
{
    // turn servo's power on
    PowerMgr.powerOn(DRIVER_POWER_PIN);

    // initialize servo driver
    pwm.begin();

    // set servo frequency
    pwm.setPWMFreq(SERVO_FREQ);

    // set servo to a fixed position on start
    pwm.setPWM(0, 0, 150);
}

//=========================================================================================================

//=========================================================================================================
// calibrate_bare_servo() - find out min and max PWM for servo when not installed
//=========================================================================================================
void CServoDriver::calibrate_bare_servo()
{
    
}

//=========================================================================================================
