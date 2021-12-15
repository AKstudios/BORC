#include "globals.h"


//=========================================================================================================
// start()
//=========================================================================================================
void CMenuMgr::start()
{   
    // set the interface mode to menu mode
    System.iface_mode = MENU;

    // set the menu item to manual
    m_menu_item = MI_MANUAL;

    // display the menu item (2 characters)
    display_item();
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
            Display.display("Ma");
            break;
        case MI_SETPOINT:
            Display.display("Se");
            break;
        case MI_CALIBRATE:
            Display.display("Ca");
            break;
        case MI_ROTATE_D:
            Display.display("Rd");
            break;
        case MI_ROOMTEMP:
            Display.display("Rt");
            break;
        case MI_REBOOT:
            Display.display("Re");
            break;
        case MI_CONFIG:
            Display.display("Co");
            break;
        case MI_DESTROY:
            Display.display("De");
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
        
        case MI_DESTROY:
            Display.fill();
            Led.set(WHITE);
            delay(1000);
            EEPROM.destroy();
            System.reboot();
            break;

        case MI_CONFIG:
            break;

    }
}
//=========================================================================================================

//=========================================================================================================
// execute() - manual mode execute
//=========================================================================================================
void CMenuMgr::execute()
{
    knob_event_t event;

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

    // in real life, we will read the sensor
    int temp = 72;

    // display the current temperature
    Display.display(temp);

    // start a 5 second timer
    timer.start(5000);

    // sit in a loop for 5 seconds until someone clicks the knob
    while (!timer.is_expired())
    {   
        // stay awake
        SleepMgr.kick_timer();

        if (Knob.get_event(&event) && event == KNOB_UP) break;
    }

    // redisplay our menu item
    display_item();
}
//=========================================================================================================
