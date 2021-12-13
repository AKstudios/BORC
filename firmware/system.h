#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#include "common.h"

class CSystem
{
public:

    void init();

    //  soft reboots the system
    void reboot();

    void return_to_run_mode();

    // what mode is the interface in - what does playing with the knob do?
    mode_t iface_mode;

    // changes system orientation
    void rotate();

    // set system-wide orientation
    void set_orientation(bool orientation);

protected:

};

#endif
