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
    void calibrate_bare_servo();

    // find out min and max PWM for servo when installed
    void calibrate_installed_servo();

    // get the highest value to send to servo class (0 to max)
    int get_max_position() {return m_max_limit - m_min_limit;}

    // takes position as argument and makes the servo move to that position
    void start_move(int position);

    // checks whether the servo is moving and returns true or false
    bool is_moving();

protected:
    
    //  soft reboots the system
    void reboot_system();

    // Maximum PWM value the servo can take


    // Minimum PWM value the servo can take
    int m_max_limit, m_min_limit;
};

//=========================================================================================================
#endif
