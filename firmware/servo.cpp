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
static Adafruit_PWMServoDriver pwm(SERVO_DRIVER_ADDRESS);
static Adafruit_INA219 ina219(CURRENT_SENSE_ADDRESS);

// define servo's PWM frequency. Depends on type of servo
#define SERVO_FREQ  50

// this is the minimum current in mA above which we consider the servo to be moving
#define MOVING_THRESHOLD  20

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

    // set some reasonable default values for limits
    m_min_limit = 90;
    m_max_limit = 550;
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
// start_move(int pwm_value) - takes position as an argument (0-max)
//=========================================================================================================
void CServoDriver::start_move(int position)
{   
    // check if position is within acceptable range
    if (position < 0 || position > get_max_position()) return;

    Serial.println(m_min_limit + position);

    // set PWM value for the servo to move
    pwm.setPWM(0, 0, m_min_limit + position);
}
//=========================================================================================================


//=========================================================================================================
// is_moving() - checks whether the servo is moving and returns true or false
//=========================================================================================================
bool CServoDriver::is_moving()
{
    return (ina219.getCurrent_mA() > MOVING_THRESHOLD);
}
//=========================================================================================================


//=========================================================================================================
// reboot_system() - soft reboots the system
//=========================================================================================================
void CServoDriver::reboot_system()
{
    // reboot system
    asm volatile ("jmp 0");
}
//=========================================================================================================