#ifndef _SERVO_H_
#define _SERVO_H_

//=========================================================================================================
// CServoDriver() - A class that manages the servo driver NXP PCA9685               
//=========================================================================================================
class CServoDriver
{
public:

    // initialize the servo driver
    void init();
    
    // find out min and max PWM for servo when not installed
    void calibrate_bare();

    // find out min and max PWM for servo when installed
    void calibrate_installed();

    // get the highest value to send to servo class (0 to max)
    int get_max_position() {return m_max_limit - m_min_limit;}

    // takes position as argument and makes the servo move to that position
    bool start_move_to_position(int position, int timeout_ms=1000);

    // checks whether the servo is moving and returns true or false
    bool is_moving();

protected:
// public:
    //  soft reboots the system
    void reboot_system();

    // takes pwm value as argument and makes the servo move
    bool start_move_to_pwm(int pwm_value, int timeout_ms=1000, bool enforce_limit=true);

    // move to a PWM value and times out if current never drops
    bool move_to_pwm(int pwm_value, int timeout_ms);

    // Minimum and maximum PWM value the servo can take
    int m_max_limit, m_min_limit;

    // current thresholds for sensing servo start and stop
    int m_start_moving_threshold, m_stop_moving_threshold;
};

//=========================================================================================================
#endif
