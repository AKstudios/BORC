//=========================================================================================================
// Menu Manager - manages menus
//=========================================================================================================
#ifndef _MENU_MANAGER_H_
#define _MENU_MANAGER_H_
#include "mstimer.h"

class CMenuMgr
{
public:

    enum    menu_item_t
    {
        MI_MANUAL   = 0,
        MI_SETPOINT,
        MI_CALIBRATE,
        MI_ROTATE_D,
        MI_ROOMTEMP,
        MI_REBOOT,
        MI_CONFIG,
        MI_COUNT
    };

    void    start();

    // Call this periodicially to allow the state machine to do its thing
    void    execute();

protected:

    int     m_menu_item;

    void    display_item();

    void    dispatch();

    void    room_temp_handler();

    void    config_mode_handler();

    // When this timer expires, we automatically exit the menu
    OneShot m_menu_timer;
    
};
//=========================================================================================================
#endif