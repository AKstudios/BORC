#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

//=========================================================================================================
// power_manager.cpp - turn devices on and off by managing pin states
//=========================================================================================================
class CPowerManager
{
public:

    // initialize power manager - set all power pins to outputs
    void init();
    
    //  turns on a specific device
    void powerOn(int pin_number);

    // turns off a specific device
    void powerOff(int pin_number);
    
};
#endif
