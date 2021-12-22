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

    // This is the number of notch values our array can hold
    enum { MAX_NOTCHES = 10};

protected:

    // Inform this object how long a single "sleep period" is for this system.  Normally 32 seconds,
    // except when simulating
    void    declare_sleep_time(uint16_t sleep_time_seconds);

    // These are the limits on the output
    nc_out_t    m_lower_limit, m_upper_limit;

    // These are the notches that we can set the output to
    nc_out_t    m_notch_value[MAX_NOTCHES];

    // A timer
    nc_time_t   m_timer, m_duration, m_started_at;

    // This is the currently output notch index
    int8_t      m_current_notch;

    // This is the highest valid notch
    int8_t      m_MAX_NOTCH;

    // These are the lower and upper boundary of the dead-band for the current setpoint
    nc_pv_t     m_lower_boundary, m_upper_boundary;

    // This is the most recently set setpoint
    nc_pv_t     m_setpoint_f;

};

#endif

