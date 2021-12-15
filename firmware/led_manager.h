//=========================================================================================================
// ledmgr.h - Defines a single three-color (R/G/B) LED
//=========================================================================================================
#ifndef _LEDMGR_H_
#define _LEDMGR_H_
#include "mstimer.h"

// Red is in bit 2, Green is in bit 1, Blue is in bit 0
enum led_color_t
{
    OFF    = 0,
    RED    = 4,
    GREEN  = 2,
    BLUE   = 1,
    AQUA   = GREEN | BLUE,
    PURPLE = RED | BLUE,
    AMBER  = RED | GREEN,
    WHITE  = RED | GREEN | BLUE
};


class CLedMgr
{
public:

    // Called once at setup to initialize the pins
    void    init();

    // Call this to set the LED to a new pattern. 
    // If period_ms = 0, LED is solid
    // If period_ms = 1, LED does a single 10ms flash
    // If period_ms is anything else, it's the blink period.
    // single_flash
    void    set(led_color_t color, unsigned int period_ms = 0, bool single_flash = false);

    // Call this periodicially to allow the state machine to do its thing
    void    execute();

protected:

    // This is the color pattern for the LED "on" condition
    led_color_t  m_color;

    // This is the timer that controls blinking
    msTimer      m_timer;

    // This will be true when the LED is on, and false when it's off
    bool         m_state;

    // This will be true if we're supposed to stop the timer after a single flash
    bool         m_is_single_flash;

    // Sets the LED pins to the specified color pattern
    void         on(led_color_t pattern);

    // Turns all of the LED pins off
    void         off();
};

#endif