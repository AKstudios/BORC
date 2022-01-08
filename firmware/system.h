#ifndef _SYSTEM_H_
#define _SYSTEM_H_
#include "common.h"

class CSystem
{
public:

    void init();

    //  soft reboots the system
    void reboot();

    // Places the interface mode in either manual mode or setpoint mode
    void return_to_run_mode();

    // changes system orientation
    void rotate();

    // set system-wide orientation
    void set_orientation(bool orientation);

    // what mode is the interface in - what does playing with the knob do?
    mode_t iface_mode;

    // The most recently read temperature in degrees F
    float   temp_f;

};

#endif
