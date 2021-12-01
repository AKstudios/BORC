#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

//=========================================================================================================
// power_manager.cpp - turn devices on and off
//=========================================================================================================
class CPowerManager
{
public:

    //  turns on a specific device
    void powerOn(int pin);

    // turns off a specific device
    void powerOff(int pin);
};
#endif
