#include "globals.h"


//=========================================================================================================
// start()
//=========================================================================================================
void CMenuMgr::start()
{   
    // Ensure that the menu timer isn't running
    m_menu_timer.stop();

    // set the interface mode to menu mode
    System.iface_mode = MENU_MODE;

    // set the menu item to manual
    m_menu_item = MI_MANUAL;

    // display the menu item (2 characters)
    display_item();

    // If we're plugged into USB, start a timer to make the menu timeout
    if (USB.current_state()) m_menu_timer.start(10000);
}
//=========================================================================================================


//=========================================================================================================
// display_item()
//=========================================================================================================
void CMenuMgr::display_item()
{
    switch (m_menu_item)
    {
        case MI_MANUAL:
            Display.print("Ma");
            break;
        case MI_SETPOINT:
            Display.print("Se");
            break;
        case MI_CALIBRATE:
            Display.print("Ca");
            break;
        case MI_ROTATE_D:
            Display.print("Rd");
            break;
        case MI_ROOMTEMP:
            Display.print("Rt");
            break;
        case MI_REBOOT:
            Display.print("Re");
            break;
        case MI_CONFIG:
            Display.print("Co");
            break;
    }
}
//=========================================================================================================

//=========================================================================================================
// dispatch()
//=========================================================================================================
void CMenuMgr::dispatch()
{
    switch (m_menu_item)
    {
        case MI_MANUAL:
            ManualModeMgr.start();
            break;

        case MI_SETPOINT:
            SetpointModeMgr.start();
            break;

        case MI_CALIBRATE:
            Servo.calibrate_bare();
            break;

        case MI_ROTATE_D:
            System.rotate();
            break;

        case MI_ROOMTEMP:
            room_temp_handler();
            break;

        case MI_REBOOT:
            System.reboot();
            break;

        case MI_CONFIG:
            break;

    }
}
//=========================================================================================================

//=========================================================================================================
// execute() - menu mode execute
//=========================================================================================================
void CMenuMgr::execute()
{
    knob_event_t event;

    // If the menu timer has expired, the exit the menu
    if (m_menu_timer.is_expired())
    {
        System.return_to_run_mode();
        return;
    }

    // Handle events from the rotary knob
    while (Knob.get_event(&event))
    {
        switch (event)
        {
        case KNOB_LEFT:
            if (m_menu_item > 0)
            {
                --m_menu_item;
                display_item();
            }
            break;

        case KNOB_RIGHT:
            if (m_menu_item < MI_COUNT-1)
            {
                ++m_menu_item;
                display_item();
            }
            break;

        case KNOB_LPRESS:
            System.return_to_run_mode();
            break;
        
        case KNOB_UP:
            dispatch();
            break;
        }

        // Any knob activity causes us to kick the menu timer
        m_menu_timer.kick();
    }
}
//=========================================================================================================


//=========================================================================================================
// room_temp_handler() - read current temperature
//=========================================================================================================
void CMenuMgr::room_temp_handler()
{   
    knob_event_t event;

    OneShot timer;

    // get temp from sensor and convert to farenheit
    float temp_f = TempHum.read_temp_f();

    // display the current temperature
    Display.print(temp_f);

    // start a 5 second timer
    timer.start(5000);

    // sit in a loop for 5 seconds until someone clicks the knob
    while (!timer.is_expired())
    {   
        // stay awake
        SleepMgr.kick_sleep_timer();

        // if someone clicks the knob, break out
        if (Knob.get_event(&event) && event == KNOB_UP) break;
    }

    // redisplay our menu item
    display_item();
}
//=========================================================================================================
