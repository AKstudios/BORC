//=========================================================================================================
// temp_ctrl.cpp - Implements a notch controller for controlling temperature
//=========================================================================================================
#include "globals.h"


const int RAMPING = -1;

//=========================================================================================================
// init() - Called once at startup
//=========================================================================================================
void CNotchController::init()
{
    // get controller limits from EEPROM
    set_output_limits(ee.servo_min, ee.servo_max);

    // The timer started at boot
    m_started_at = 0;

    // The timer starts out expired so that the first call to compute() controls the output
    m_timer = TIMER_SECONDS + 1;

    // Set the duration of the timer, in seconds
    m_duration = TIMER_SECONDS;

    // Assume for the moment that we don't know where our output is
    m_current_notch = RAMPING;

    // And we don't have a setpoint yet
    m_setpoint_c = 0;
}
//=========================================================================================================



//=========================================================================================================
// reset() - Tells the controller that the output control has been changed outside of our control
//=========================================================================================================
void CNotchController::reset() { m_current_notch = RAMPING; }
//=========================================================================================================


//=========================================================================================================
// set_output_limits() Define the minimum and maximum legal values for the output
//=========================================================================================================
void CNotchController::set_output_limits(nc_out_t lower_limit, nc_out_t upper_limit)
{
    m_lower_limit = lower_limit;
    m_upper_limit = upper_limit;

    Serial.print("Limits ");
    Serial.print(lower_limit);
    Serial.print("  ");
    Serial.print(upper_limit);
    Serial.println();

    // Determine the size of the increment between notches
    float increment = (upper_limit - lower_limit) / float(NOTCH_COUNT - 1);
    
    // Set the output values of the notches
    for (int i=0; i<MAX_NOTCH; ++i)
    {
        m_notch_value[i] = lower_limit + (nc_out_t)(i * increment + .5);
        Serial.print("Notch: ");
        Serial.print(i);
        Serial.print(" = ");
        Serial.println(m_notch_value[i]);
    }
    
    // Ensure that the highest notch is exactly the upper limit
    m_notch_value[MAX_NOTCH] = upper_limit;

    // print the last notch
    Serial.print("Notch: ");
    Serial.print(MAX_NOTCH);
    Serial.print(" = ");
    Serial.println(m_notch_value[MAX_NOTCH]);


}
//=========================================================================================================



//=========================================================================================================
// new_setpoint_f() - Starts a new setpoint.
//=========================================================================================================
void CNotchController::new_setpoint_f(nc_pv_t setpoint_f)
{
    // What setpoint are we being asked to set?
    float setpoint_c = f_to_c(setpoint_f);

    // If that's different than our current setpoint, spring into action
    if (setpoint_c != m_setpoint_c)
    {
        // Save the setpoint for posterity
        m_setpoint_c = setpoint_c;
        
        // Compute the in-band boundaries
        m_lower_boundary = setpoint_c - DEADBAND_SIZE;
        m_upper_boundary = setpoint_c + DEADBAND_SIZE;
        
        // Switch to ramping mode
        m_current_notch  = RAMPING;
    }
}
//=========================================================================================================



//=========================================================================================================
// compute() - Compute a new output value
// 
// Passed: pv = Present value of the thing being controlled (temperature, etc)
//         dt = Amount of time that has elapsed since the last time this was called
//
//=========================================================================================================
bool CNotchController::compute(nc_pv_t pv_c, nc_time_t dt, nc_out_t* p_output)
{
    // Update the timer
    m_timer += dt;

    // How much time has elapsed since the timer was last started?
    nc_time_t elapsed = m_timer - m_started_at;

    // If the timer has not yet expired, do nothing
    if (elapsed < m_duration) return false;

    // Restart the timer
    m_started_at += m_duration;

    // Save the original notch value when we enter this routine
    int8_t original_notch = m_current_notch;    

    // If we're in ramping mode...
    if (m_current_notch == RAMPING)
    {
        // If the temperature is too low, turn the output all the way on
        if (pv_c < m_lower_boundary)
        {
            m_current_notch = MAX_NOTCH;
            *p_output = m_notch_value[m_current_notch];
        }

        // If the temperatue is too high, turn the output all the way off
        else if (pv_c > m_upper_boundary)
        {
            m_current_notch = 0;
            *p_output = m_notch_value[m_current_notch];
        }

        // We're done managing ramping
        goto cleanup;
    }


    // If the pv is too low and we have room to move the output up a notch, do so
    if (pv_c < m_lower_boundary && m_current_notch < MAX_NOTCH)
    {
        *p_output = m_notch_value[++m_current_notch];
    }

    // If the pv is too high and we have room to bump the output down a notch, do so
    else if (pv_c > m_upper_boundary && m_current_notch > 0)
    {
        *p_output = m_notch_value[--m_current_notch];
    }

cleanup:

    // Had the current notch changed?
    bool change_occured = (m_current_notch != original_notch);

    // Change this "#if 0" to a "#if 1" to see debugging data as the notch changes
    #if 1
    if (change_occured)
    {
        if (original_notch == RAMPING)
        {
            Serial.println(m_current_notch == 0 ? "Servo railed LOW" : "Servo railed HIGH");
        }
        else
        {
            Serial.print("Servo bumped from notch ");
            Serial.print(original_notch);
            Serial.print(" to ");
            Serial.print(m_current_notch);
            Serial.println();
        }

    }
    #endif


    // Tell the caller whether or not he should perform a motor movement
    return change_occured;
}
//=========================================================================================================
