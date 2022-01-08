#ifndef _SERVO_H_
#define _SERVO_H_

#define UNKNOWN_POSITION    -1
#define DEFAULT_MIN_LIMIT   3595
#define DEFAULT_MAX_LIMIT   3930
#define MAX_INDEX           6

//=========================================================================================================
// CServoDriver() - A class that manages the servo driver NXP PCA9685               
//=========================================================================================================
class CServoDriver
{
public:

    // initialize the servo driver
    void init();

    // reinitialize the servo stuff
    void reinit();
    
    // find out min and max PWM for servo when not installed
    void calibrate_bare();

    // find out min and max PWM for servo when installed
    void calibrate_installed();

    // manual mode index are 0 to this value
    int get_max_index() {return MAX_INDEX;}

    // takes a manual index move the servo to it
    bool move_to_index(int index);

    // move to a PWM value and times out if current never drops
    bool move_to_pwm(int pwm_value, int timeout_ms, bool enforce_limit=true);

    // checks whether the servo is moving and returns true or false
    bool is_moving();

    // Return the current PWM value the servo motor is parked at
    uint16_t current_pwm() {return m_current_pwm;}

protected:
//  public:

    // Determines whether "move to pwm" will automatically control power to the servo
    enum pwr_ctrl_t {MANUAL, AUTO} m_power_control;
    
    // Call these to push and pop power control settings
    void push_power_control(pwr_ctrl_t new_setting);
    void pop_power_control();

    // takes pwm value as argument and makes the servo move
    bool start_move_to_pwm(int pwm_value, bool enforce_limit=true);

    // servo may jitter after moving to a position, this code waits for it to stop
    bool wait_for_servo_to_settle();

    // current thresholds for sensing servo start and stop
    int m_start_moving_threshold, m_stop_moving_threshold;

    // current and target pwm position of the motor
    int m_current_pwm, m_target_pwm;

    // keeps track of how many times we see the stop threshold current
    int m_stop_current_counter;

    // A push-able/pop-able stack of power control settings
    uint16_t m_power_control_stack;
};

//=========================================================================================================
#endif
