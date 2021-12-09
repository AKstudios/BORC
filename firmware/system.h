#ifndef _SYSTEM_H_
#define _SYSTEM_H_

enum mode_t {MANUAL, MENU, SETPOINT};

class CSystem
{
public:

    void init();

    int manual_index;

    int setpoint;

    //  soft reboots the system
    void reboot();

    void return_to_run_mode();

    // manual or setpoint mode
    mode_t run_mode;

    // what mode is the interface in - what does playing with the knob do?
    mode_t iface_mode;

    // changes system orientation
    void rotate();

protected:

    bool m_orientation;
};

#endif
