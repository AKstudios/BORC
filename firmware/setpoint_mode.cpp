#include "globals.h"


//=========================================================================================================
// int() - initialize at boot
//=========================================================================================================
void CSetpointModeMgr::init()
{   
    m_setpoint_f = c_to_f(ee.setpoint);
}
//=========================================================================================================


//=========================================================================================================
// start() - start setpoint mode
//=========================================================================================================
void CSetpointModeMgr::start()
{   
    
    ee.run_mode = System.iface_mode = SETPOINT;

    // display the current setpoint
    Display.display(m_setpoint_f);
}
//=========================================================================================================


//=========================================================================================================
// execute() - setpoint mode state machine
//=========================================================================================================
void CSetpointModeMgr::execute()
{
    knob_event_t event;

    while (Knob.get_event(&event))
    {
        
        switch (event)
        {
        case KNOB_LEFT:
            if (m_setpoint_f > MIN_SETPOINT)
            {
                --m_setpoint_f;
                Display.display(m_setpoint_f);
                Led.set(BLUE, 1000, true);
                ee.setpoint = f_to_c(m_setpoint_f);
                PID.new_setpoint(ee.setpoint);
            }
            break;

        case KNOB_RIGHT:
            if (m_setpoint_f < MAX_SETPOINT)
            {
                ++m_setpoint_f;
                Display.display(m_setpoint_f);
                Led.set(RED, 1000, true);
                ee.setpoint = f_to_c(m_setpoint_f);
                PID.new_setpoint(ee.setpoint);
            }
            break;
        
        case KNOB_LPRESS:
            MenuMgr.start();
            break;
        }
    }
}
