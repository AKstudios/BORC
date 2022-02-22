//=========================================================================================================
// CServoDriver() - A class that manages the servo driver NXP PCA9685               
//=========================================================================================================
#include "globals.h"
#include <Adafruit_PWMServoDriver.h>  //https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

// initialize all library objects
static Adafruit_PWMServoDriver pwm(SERVO_DRIVER_ADDRESS);

// define servo's PWM frequency. Depends on type of servo
#define SERVO_FREQ  50

// This is the maximum valid PWM value (4095 = 12-bit)
#define MAX_VALID_PWM 4095

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

    // set servo frequency
    pwm.setPWMFreq(SERVO_FREQ);

    // set start and stop current thresholds in mA (no load on motor)
    m_start_moving_threshold = 300;
    m_stop_moving_threshold = 200;
    
    // set current pwm to a default unknown value
    m_current_pwm = UNKNOWN_POSITION;

    // set stop current counter to a default known value
    m_stop_current_counter = 0;

    // give the system power control of the servo
    m_power_control = AUTO;

    // if servo isn't calibrated, use defaults
    if (ee.is_servo_calibrated != CAL)
    {
        ee.servo_max = DEFAULT_MAX_LIMIT;
        ee.servo_min = DEFAULT_MIN_LIMIT;
    }
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
}

//=========================================================================================================
// calibrate_bare() - find out min and max PWM for servo when not installed
//=========================================================================================================
void CServoDriver::calibrate_bare()
{   
    // show that we're calibrating the servo on the display and LED
    Display.print("Ca");
    Led.set(PURPLE);

    // assume this is going to work
    bool success = true;

    // Switch to manual power control
    push_power_control(MANUAL);
    
    // If there's no hardware attached, just use default calibration values
    if (NO_HW) 
    {
        ee.servo_min = DEFAULT_MIN_LIMIT;
        ee.servo_max = DEFAULT_MAX_LIMIT;
        goto cleanup;        
    }

    // manually turn on servo's power
    PowerMgr.powerOn(SERVO_POWER_PIN);

    // set known values to all limits
    const int safe_lo_pwm = (DEFAULT_MIN_LIMIT + DEFAULT_MAX_LIMIT)/2;
    const int safe_hi_pwm = (DEFAULT_MIN_LIMIT + DEFAULT_MAX_LIMIT)/2;
    const int dangerous_lo_pwm = 3345;
    const int dangerous_hi_pwm = 4095;
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
        // Perform current logging
        CurLogger.execute();

        // move our target down by a step
        current_target -= step_size;

        // have we failed to find the lower limit?
        if (current_target <= dangerous_lo_pwm) 
        {  
            // then set it to default
            ee.servo_min = DEFAULT_MIN_LIMIT;
            success = false;
            break;
        }

        // wait for the servo to stop moving from previous move
        wait_for_servo_to_settle();

        // if the movement to this target doesn't start
        if (!move_to_pwm(current_target, 4000, false))
        {   
            // we found our lower limit
            ee.servo_min = current_target + step_size;
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
        // Perform current logging
        CurLogger.execute();

        // move our target up by a step
        current_target += step_size;

        // have we failed to find the higher limit?
        if (current_target >= dangerous_hi_pwm) 
        {   
            // then set it to default
            ee.servo_max = DEFAULT_MAX_LIMIT;
            success = false;
            break;
        }

        // wait for the servo to stop moving from previous move
        wait_for_servo_to_settle();

        // if the movement to this target doesn't start
        if (!move_to_pwm(current_target, 4000, false))
        {
            // we found our upper limit
            ee.servo_max = current_target - step_size;
            break;
        }
    }

cleanup:

    // manually turn off servo's power
    PowerMgr.powerOff(SERVO_POWER_PIN);

    // Switch back to the previous power control setting
    pop_power_control();

    // if calibration is successful..
    if (success)
    {   
        // store servo calibration status in EEPROM
        ee.is_servo_calibrated = CAL;
        
        // and set output limits
        TempCtrl.set_output_limits(ee.servo_min, ee.servo_max);
    }

    // turn LED off after calibration
    Led.set(OFF);

    // tell sleep manager that we moved the motor without his permission.. sorry!
    SleepMgr.marked_motor_as_moved();
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
    bool status = true;

    // If we don't have any hardware attached, we're done
    if (NO_HW) return true;

    // turn on servo's power if needed
    if (m_power_control == AUTO) PowerMgr.powerOn(SERVO_POWER_PIN);

    // create a oneshot timer
    OneShot servo_timer;

    // start moving the servo - converting the PWM value into a position
    bool is_move_started = start_move_to_pwm(pwm_value, enforce_limit);

    // if servo hasn't started to move, return false
    if (!is_move_started)
    {
        status = false;
        goto cleanup;
    }
    // start the oneshot timer again
    servo_timer.start(timeout_ms);

    // sit in a loop until the timer expires while servo moves
    while (is_moving())
    {   
        // Perform current logging
        CurLogger.execute();

        // check the timer
        if (servo_timer.is_expired())
        {   
            // if it is expired, we don't know where the servo is
            m_current_pwm = UNKNOWN_POSITION;

            // cleanup and tell the caller
            status = false;
            goto cleanup;
        }
    }

cleanup:

    // turn off servo's power if needed
    if (m_power_control == AUTO) PowerMgr.powerOff(SERVO_POWER_PIN);

    // Perform current logging
    CurLogger.execute();

    // once it's done moving, tell the caller that the servo moved
    return status;
}
//=========================================================================================================


//=========================================================================================================
// start_move() - takes position as an argument (0-max)
// returns true when servo starts to move, false if it doesn't move at all
//=========================================================================================================
bool CServoDriver::start_move_to_pwm(int pwm_value, bool enforce_limit)
{   
    // Start by checking bus voltage
    // int16_t bus_mv, shunt_mv;
    // INA219.get_bus_mv(&bus_mv);
    // INA219.get_shunt_mv(&shunt_mv);
    
    // // If servo power voltage is out of range, set error bit
    // Serial.println(shunt_mv);
    // if (bus_mv < 2500 || bus_mv > 6000)
    //     System.error_byte |= SERVO_POWER_ERR;

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
        if (pwm_value < ee.servo_min || pwm_value > ee.servo_max) return false;
    }

    // Perform current logging
    CurLogger.execute();

    // set PWM value for the servo to move
    pwm.setPWM(0, 0, MAX_VALID_PWM - pwm_value);

    // start the oneshot timer, it shouldn't take more than 1 second to start a move
    servo_timer.start(1000);

    // sit in a loop waiting for the servo to start moving
    while (!servo_timer.is_expired())
    { 
        // fetch the servo current
        int current = INA219.get_current_ma();

        // Perform current logging
        CurLogger.execute(current);

        // here we're checking to see if current is above the threshold three times in a row
        if (current > m_start_moving_threshold)
        {   
            if (++current_counter >= 3)
            {
                // if we get here, there are no errors
                System.error_byte &= ~SERVO_ERR;
                return true;
            }
        }
      
        // otherwise, start over
        else current_counter = 0;
    }

    // if we get here it never started moving. Set error byte and tell the caller
    System.error_byte |= SERVO_ERR;
    return false;
}
//=========================================================================================================


//=========================================================================================================
// move_to_index() - moves the servo to an exact position based on the index
//=========================================================================================================
bool CServoDriver::move_to_index(int index)
{   
    int range = ee.servo_max - ee.servo_min;
    
    int pwm_value = index * range / get_max_index() + ee.servo_min;

    return move_to_pwm(pwm_value, 4000, true);
}
//=========================================================================================================


//=========================================================================================================
// is_moving() - checks whether the servo is moving and returns true or false
//=========================================================================================================
bool CServoDriver::is_moving()
{   
    // get current draw of servo
    int current = INA219.get_current_ma();

    // Perform current logging
    CurLogger.execute(current);
    
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
    else m_stop_current_counter = 0;

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
        int current = INA219.get_current_ma();

        // Perform current logging
        CurLogger.execute(current);

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


//=========================================================================================================
// push_power_control() - Saves the current power control setting and sets a new one
//=========================================================================================================
void CServoDriver::push_power_control(pwr_ctrl_t new_setting)
{
    // Save the existing power control setting
    m_power_control_stack = (m_power_control_stack << 1) | m_power_control;

    // And start using the new power control setting
    m_power_control = new_setting;
}
//=========================================================================================================


//=========================================================================================================
// pop_power_control() - Restores a previously pushed power control setting
//=========================================================================================================
void CServoDriver::pop_power_control()
{
    // Start using the power control setting that was at the top of the stack
    m_power_control = (pwr_ctrl_t)(m_power_control_stack & 1);

    // And pop one entry off the stack
    m_power_control_stack >>= 1;
}
//=========================================================================================================



