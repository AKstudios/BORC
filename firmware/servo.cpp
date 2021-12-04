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

    // set start and stop current thresholds in mA (no load on motor)
    m_start_moving_threshold = 20;
    m_stop_moving_threshold = 50;
}
//=========================================================================================================


//=========================================================================================================
// calibrate_bare_servo() - find out min and max PWM for servo when not installed
//=========================================================================================================
void CServoDriver::calibrate_bare()
{

}
//=========================================================================================================


//=========================================================================================================
// calibrate_installed_servo() - check if the servo hits a wall when control is installed on the valve
//=========================================================================================================
void CServoDriver::calibrate_installed()
{
    
}
//=========================================================================================================


//=========================================================================================================
// move_to_pwm() - move to a PWM value and times out if current never drops
// returns true if move completes, returns false if it doesn't
//=========================================================================================================
bool CServoDriver::move_to_pwm(int pwm_value, int timeout_ms)
{
    // create a oneshot timer
    OneShot servo_timer;

    // start moving the servo - converting the PWM value into a position
    bool is_move_started = start_move(pwm_value - m_min_limit);

    // if servo hasn't started to move, return false
    if (!is_move_started) return false;
    
    // start the oneshot timer again
    servo_timer.start(timeout_ms);
    
    // sit in a loop until the timer expires while servo moves
    while (!servo_timer.is_expired())
    {   
        // if servo is done moving, tell the caller
        if (!is_moving()) break;
    }

    // once it's done moving, tell the caller that the servo moved
    return true;
}
//=========================================================================================================


//=========================================================================================================
// start_move() - takes position as an argument (0-max)
// returns true when servo starts to move, false if it doesn't move at all
//=========================================================================================================
bool CServoDriver::start_move(int position, int timeout_ms)
{   
    // keep track of valid sequential current readings
    int current_counter = 0;
    
    // create a oneshot timer
    OneShot servo_timer;

    // check if position is within acceptable range
    if (position < 0 || position > get_max_position()) return false;
  
    // set PWM value for the servo to move
    pwm.setPWM(0, 0, m_min_limit + position);

    // start the oneshot timer
    servo_timer.start(timeout_ms);

    // sit in a loop waiting for the servo to start moving
    while (!servo_timer.is_expired())
    { 
      // fetch the servo current
      float current = ina219.getCurrent_mA();

      // here we're checking to see if current is above 50mA three times in a row
      if (current > m_start_moving_threshold)
      {
        if (++current_counter >= 3) return true;
      }
      
      // otherwise, start over
      else current_counter = 0;
    }

    // if we get here it never started moving
    return false;
}
//=========================================================================================================


//=========================================================================================================
// is_moving() - checks whether the servo is moving and returns true or false
//=========================================================================================================
bool CServoDriver::is_moving()
{   
    float current = ina219.getCurrent_mA();
//    Serial.println(current);
    return (current > m_stop_moving_threshold);
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
