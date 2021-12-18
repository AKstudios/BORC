//=========================================================================================================
// pid_ctrl.h - Defines a classic PID controller
//=========================================================================================================
#ifndef _PID_CTRL_H_
#define _PID_CTRL_H_

typedef float pid_t;

//=========================================================================================================
// At system startup, the following sequence of events must happen:
// 
//     PID.set_constants(....);
//     PID.set_output_limits(...);
//     PID.new_setpoint(...)
// 
// Periodically call compute().
//=========================================================================================================
class CPIDController
{
public:

    // Constructor
    CPIDController() { reset(); }

    // initialize at boot
    void    init();

    // Clears the integral term
    void    reset();

    // Call this to set the output limits to a physically possible range
    void    set_output_limits(pid_t lower_limit, pid_t upper_limit);

    // Call this to start a new setpoint.  setpoint is in degrees F
    void    new_setpoint_f(int setpoint_f);

    // Stores new PID constants.  This resets the integral
    void    set_constants(pid_t kp, pid_t ki, pid_t kd);

    // Call this to compute a new output value. 
    pid_t   compute(pid_t pv, pid_t dt);

protected:

    // This is the measured value we are trying to maintain control of
    pid_t   m_setpoint_c;

    // This is the accumulated sum of errors
    pid_t   m_integral;

    // These are the tunable kP, kI, and kD constants
    pid_t   m_kp, m_ki, m_kd;

    // These are the limits on the output
    pid_t   m_lower_limit, m_upper_limit;

    // This is the error saved from the most recent called to "compute()"
    pid_t   m_previous_error;

};

#endif

