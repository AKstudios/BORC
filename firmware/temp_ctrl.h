//=========================================================================================================
// temp_ctrl.h - Defines a notch controller for controlling temperature
//=========================================================================================================
#ifndef _TEMP_CTRL_H_
#define _TEMP_CTRL_H_
#include "common.h"


typedef float    nc_pv_t;
typedef uint16_t nc_out_t;
typedef uint32_t nc_time_t;

//=========================================================================================================
// At system startup, the following sequence of events must happen:
// 
//     TempController.init()
//     TempController.set_output_limits(...);
//     TempController.new_setpoint(...)
// 
// Periodically call compute().
//=========================================================================================================
class CNotchController
{
public:

    // One time initialization
    void    init();
    
    // Tell the controller that the output has been changed without its permission
    void    reset();

    // Call this to set the output limits to a physically possible range
    void    set_output_limits(nc_out_t lower_limit, nc_out_t upper_limit);

    // Call this to start a new setpoint.  
    void    new_setpoint_f(nc_pv_t setpoint_f);
        
    // Call this to compute a new output value. 
    bool    compute(nc_pv_t pv, nc_time_t dt, nc_out_t* p_output);

protected:

    // The number of different notches we can set our output to
    enum        { NOTCH_COUNT = 2 };

    // The index of the highest valid notch
    enum        { MAX_NOTCH = NOTCH_COUNT - 1 };

    // How often "compute" emits a new output setting
    enum        { TIMER_SECONDS = WAKEUP_TIME_SECS * 2};

    // This is the size of the dead-band above and below the setpoint
    const nc_pv_t DEADBAND_SIZE = 1;

    // These are the limits on the output
    nc_out_t    m_lower_limit, m_upper_limit;

    // These are the notches that we can set the output to
    nc_out_t    m_notch_value[NOTCH_COUNT];

    // A timer
    nc_time_t   m_timer, m_duration, m_started_at;

    // This is the currently output notch index
    int8_t      m_current_notch;

    // These are the lower and upper boundary of the dead-band for the current setpoint
    nc_pv_t     m_lower_boundary, m_upper_boundary;

    // This is the most recently set setpoint
    nc_pv_t     m_setpoint_c;

};

#endif

