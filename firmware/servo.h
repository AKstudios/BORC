#ifndef _SERVO_H_
#define _SERVO_H_

#define UNKNOWN_POSITION    -1
#define DEFAULT_MIN_LIMIT   165
#define DEFAULT_MAX_LIMIT   500
#define MAX_INDEX           6

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

    // manual mode index are 0 to this value
    int get_max_index() {return MAX_INDEX;}

    // takes position as argument and makes the servo move to that position
    bool start_move_to_position(int position);

    // takes a manual index move the servo to it
    bool move_to_index(int index);

    // checks whether the servo is moving and returns true or false
    bool is_moving();

protected:
 public:
    //  soft reboots the system
    void reboot_system();

    // takes pwm value as argument and makes the servo move
    bool start_move_to_pwm(int pwm_value, bool enforce_limit=true);

    // move to a PWM value and times out if current never drops
    bool move_to_pwm(int pwm_value, int timeout_ms, bool enforce_limit);

    // servo may jitter after moving to a position, this code waits for it to stop
    bool wait_for_servo_to_settle();

    // Minimum and maximum PWM value the servo can take
    int m_max_limit, m_min_limit;

    // current thresholds for sensing servo start and stop
    int m_start_moving_threshold, m_stop_moving_threshold;

    // current and target pwm position of the motor
    int m_current_pwm, m_target_pwm;

    // keeps track of how many times we see the stop threshold current
    int m_stop_current_counter;
};

//=========================================================================================================
#endif
