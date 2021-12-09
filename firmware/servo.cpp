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
//    // turn servo's power on
//    PowerMgr.powerOn(DRIVER_POWER_PIN);
//
//    // turn current sensor's power on
//    PowerMgr.powerOn(CURRENT_SENSE_POWER_PIN);

    // initialize servo driver
    pwm.begin();

    // initialize current sensor
    ina219.begin();

    // set servo frequency
    pwm.setPWMFreq(SERVO_FREQ);

    // set some reasonable default values for limits
    m_min_limit = DEFAULT_MIN_LIMIT;
    m_max_limit = DEFAULT_MAX_LIMIT;

    // set start and stop current thresholds in mA (no load on motor)
    m_start_moving_threshold = 300;
    m_stop_moving_threshold = 200;
    
    // set current pwm to a default unknown value
    m_current_pwm = UNKNOWN_POSITION;

    // set stop current counter to a default known value
    m_stop_current_counter = 0;
}
//=========================================================================================================

//=========================================================================================================
// reinit() - reinitialize the servo driver and current chip
//=========================================================================================================
void CServoDriver::reinit()
{
    
    // initialize servo driver
    pwm.begin();

    // set servo frequency
    pwm.setPWMFreq(SERVO_FREQ);

    // initialize current sensor
    ina219.begin();
}

//=========================================================================================================
// calibrate_bare() - find out min and max PWM for servo when not installed
//=========================================================================================================
void CServoDriver::calibrate_bare()
{
    // set known values to all limits
    const int safe_lo_pwm = (DEFAULT_MIN_LIMIT + DEFAULT_MAX_LIMIT)/2;
    const int safe_hi_pwm = (DEFAULT_MIN_LIMIT + DEFAULT_MAX_LIMIT)/2;
    const int dangerous_lo_pwm = 0;
    const int dangerous_hi_pwm = 750;
    const int step_size = 10;

    // wait for the servo to stop moving
    wait_for_servo_to_settle();

    // move to a safe position
    move_to_pwm(safe_lo_pwm, 4000, false);
    
    // start feeling for the lower limit from here
    int current_target = safe_lo_pwm;

    // until we find the limit..
    while (true)
    {   
        // move our target down by a step
        current_target -= step_size;

        // have we failed to find the lower limit?
        if (current_target <= dangerous_lo_pwm) 
        {   
            // then set it to default
            m_min_limit = DEFAULT_MIN_LIMIT;
            break;
        }

        // wait for the servo to stop moving from previous move
        wait_for_servo_to_settle();

        // if the movement to this target doesn't start
        if (!move_to_pwm(current_target, 4000, false))
        {   
            // we found our lower limit
            m_min_limit = current_target + step_size;
            break;
        }
    }

    // wait for the servo to stop moving from previous move
    wait_for_servo_to_settle();

    // move to a safe position
    move_to_pwm(safe_hi_pwm, 4000, false);
    
    // start feeling for the higher limit from here
    current_target = safe_hi_pwm;

    // until we find the limit..
    while (true)
    {   
        // move our target up by a step
        current_target += step_size;

        // have we failed to find the higher limit?
        if (current_target >= dangerous_hi_pwm) 
        {   
            // then set it to default
            m_max_limit = DEFAULT_MAX_LIMIT;
            break;
        }

        // wait for the servo to stop moving from previous move
        wait_for_servo_to_settle();

        // if the movement to this target doesn't start
        if (!move_to_pwm(current_target, 4000, false))
        {
            // we found our upper limit
            m_max_limit = current_target - step_size;
            break;
        }
    }
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
bool CServoDriver::move_to_pwm(int pwm_value, int timeout_ms, bool enforce_limit)
{
    // create a oneshot timer
    OneShot servo_timer;

    // start moving the servo - converting the PWM value into a position
    bool is_move_started = start_move_to_pwm(pwm_value, enforce_limit);

    // if servo hasn't started to move, return false
    if (!is_move_started) return false;
    
    // start the oneshot timer again
    servo_timer.start(timeout_ms);
    
    // sit in a loop until the timer expires while servo moves
    while (is_moving())
    {   
        // check the timer
        if (servo_timer.is_expired())
        {   
            // if it is expired, we don't know where the servo is
            m_current_pwm = UNKNOWN_POSITION;

            // tell the caller
            return false;
        }
    }

    // once it's done moving, tell the caller that the servo moved
    return true;
}
//=========================================================================================================


//=========================================================================================================
// start_move() - takes position as an argument (0-max)
// returns true when servo starts to move, false if it doesn't move at all
//=========================================================================================================
bool CServoDriver::start_move_to_pwm(int pwm_value, bool enforce_limit)
{   
    // save the target pwm of where we want to move
    m_target_pwm = pwm_value;
    
    // keep track of valid sequential current readings
    int current_counter = 0;
    m_stop_current_counter = 0;
    
    // create a oneshot timer
    OneShot servo_timer;

    // if you want to enforce the PWM limits...
    if (enforce_limit)
    {
        // ...check if position is within acceptable range
        if (pwm_value < m_min_limit || pwm_value > m_max_limit) return false;
    }
  
    // set PWM value for the servo to move
    pwm.setPWM(0, 0, pwm_value);

    // start the oneshot timer, it shouldn't take more than 1 second to start a move
    servo_timer.start(1000);

    // sit in a loop waiting for the servo to start moving
    while (!servo_timer.is_expired())
    { 
      // fetch the servo current
      float current = ina219.getCurrent_mA();

      // here we're checking to see if current is above the threshold three times in a row
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
// start_move_to_position() - takes position as an argument (0-max)
// returns true when servo starts to move, false if it doesn't move at all
//=========================================================================================================
bool CServoDriver::start_move_to_position(int position)
{   
    return start_move_to_pwm(position + m_min_limit);
}
//=========================================================================================================


//=========================================================================================================
// move_to_index() - moves the servo to an exact position based on the index
//=========================================================================================================
bool CServoDriver::move_to_index(int index)
{   
    int range = m_max_limit - m_min_limit;

    // invert the index so 0 means close and 6 means open
    int effective_index = get_max_index() - index;
    
    int pwm_value = effective_index * range / get_max_index() + m_min_limit;

    return move_to_pwm(pwm_value, 4000, true);
}
//=========================================================================================================


//=========================================================================================================
// is_moving() - checks whether the servo is moving and returns true or false
//=========================================================================================================
bool CServoDriver::is_moving()
{   
    // get current draw of servo
    float current = ina219.getCurrent_mA();
    
    // does the current level look like we stopped moving?
    if (current < m_stop_moving_threshold)
    {   
        // if this happens enough times in a row
        if (++m_stop_current_counter >= 50)
        {   
            // we're stopped; save our location
            m_current_pwm = m_target_pwm;
            return false;
        }
    }

    // otherwise start counting from 0 again
    else    m_stop_current_counter = 0;

    // tell the caller we're still moving
    return true;
}
//=========================================================================================================

//=========================================================================================================
// wait_for_servo_to_settle() - servo may jitter after moving to a position, this code waits for it to stop
//=========================================================================================================
bool CServoDriver::wait_for_servo_to_settle()
{
    int current_counter = 0;
    
    // create a oneshot timer
    OneShot servo_timer;

    // start the oneshot timer
    servo_timer.start(1000);

    // sit in a loop waiting for the servo to stop jittering
    while (!servo_timer.is_expired())
    {
      // fetch the servo current
      float current = ina219.getCurrent_mA();

      // here we're checking to see if current is below 20mA multiple times in a row
      if (current < 20)
      {
          if (++current_counter >= 200) return true;
      }
      
      // otherwise, start over
      else current_counter = 0;
    }

    // if we reach here, servo never really settled
    return false;
}
//=========================================================================================================
