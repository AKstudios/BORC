//=========================================================================================================
// CServoDriver() - A class that manages the servo driver NXP PCA9685               
//=========================================================================================================
#include "servo.h"
#include "common.h"
#include "Arduino.h"
#include "globals.h"
#include <Adafruit_PWMServoDriver.h>  //https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library
#include <Adafruit_INA219.h>          //https://github.com/adafruit/Adafruit_INA219

// initialize all library objects
static Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(SERVO_DRIVER_ADDRESS);
static Adafruit_INA219 ina219(CURRENT_SENSE_ADDRESS);

// define servo's PWM frequency. Depends on type of servo
#define SERVO_FREQ  50

//=========================================================================================================
// init() - initialize the servo driver
//=========================================================================================================
void CServoDriver::init()
{
    // turn servo's power on
    PowerMgr.powerOn(DRIVER_POWER_PIN);

    // turn current sensor's power on
    PowerMgr.powerOn(CURRENT_SENSE_POWER_PIN);

    // initialize servo driver
    pwm.begin();

    // initialize current sensor
    ina219.begin();

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


//=========================================================================================================
// calibrate_installed_servo() - check if the servo hits a wall when control is installed on the valve
//=========================================================================================================
void CServoDriver::calibrate_installed_servo()
{
    
}

//=========================================================================================================


//=========================================================================================================
// get_max_position() - get the highest value to send to servo class (0 to max)
//=========================================================================================================
void CServoDriver::get_max_position()
{
    
}

//=========================================================================================================


//=========================================================================================================
// start_move(int pwm_value) - takes servo PWM value as an argument
//=========================================================================================================
void CServoDriver::start_move(int position)
{
    
}

//=========================================================================================================


//=========================================================================================================
// is_moving() - checks whether the servo is moving and returns true or false
//=========================================================================================================
bool CServoDriver::is_moving()
{
    
}

//=========================================================================================================


//=========================================================================================================
// reboot_system() - soft reboots the system
//=========================================================================================================
bool CServoDriver::reboot_system()
{
    // reboot system
    asm volatile ("jmp 0");
}

//=========================================================================================================