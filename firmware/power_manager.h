#ifndef _POWER_MANAGER_H_
#define _POWER_MANAGER_H_

//=========================================================================================================
// power_manager.h - turn devices on and off by managing pin states
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

    // turn power on for all devices
    void powerOnAll(); 

    // turn power off for all devices
    void powerOffAll();
};
//=========================================================================================================

#endif
