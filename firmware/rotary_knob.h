#ifndef _ROTARY_KNOB_H
#define _ROTARY_KNOB_H

#include "mstimer.h"

//=========================================================================================================
// These are the types of events a user can generate by manipulating the rotary knob
//=========================================================================================================
enum knob_event_t : char
{
  KNOB_LEFT,
  KNOB_RIGHT,
  KNOB_UP,
  KNOB_LPRESS
};
//=========================================================================================================



//=========================================================================================================
// CRotaryKnob() - A class that manages a rotary knob that the user can turn left, turn right, or
//                 click on
//=========================================================================================================
class CRotaryKnob
{
public:

    // Call once to initialize the knob
    void            init(int A_pin, int B_pin, int click_pin); 

    // Call this often to allow debounced event to be added to the queue
    void            execute();

    // Fetches the next available knob event.  Call this often enough to keep up with events
    bool            get_event(knob_event_t* p_event);  

    // Call this to throw away and ignore the next event that gets debounced
    void            throw_away_next_event() { m_throw_away_next_event = true; }

    // Set the left/right orientation.  true = normal, false = upside-down
    void            set_orientation(bool flag) { m_orientation = flag; }

public:

    // This routine is called by the ISR every time we sense a rotation interrupt
    void            on_rotate_interrupt();

    // This routine is called by the ISR every time we sense a button-click interrupt
    void            on_click_interrupt();

protected:

    // start deboucing an event
    void            start_debounce_timer(knob_event_t event);

    // We will keep a queue of a maximum of four pending events
    enum {MAX_EVENTS = 4};

    // Our ISR calls this to add an event to the list of pending events
    void            add_event(knob_event_t event);

    // Configure the INT interrupt registers for the rotary A-channel input pin
    void            configure_INT(int pin, bool enable_pullup);

    // Configure the PCINT interrupt registers for the click-button input put
    void            configure_PCINT(int pin);

    // The button ISR starts this timer every time is senses a button press
    ThreadSafeOneShot m_debounce_timer;

    // This timer expires when the knob is being pressed and held for a certain amount of time
    OneShot         m_button_timer;

    // How many pending events are there?
    char            m_event_count;

    // If this is true, the next "add_event()" won't add the event to the queue
    bool            m_throw_away_next_event;

    // The list of pending knob events
    knob_event_t    m_event[MAX_EVENTS];

    // debounce event
    knob_event_t    m_debounce_event;

    // Index where the next event will be added
    int             m_put_index;

    // Index where the next event will be fetched from
    int             m_get_index;

    // The pins numbers for rotary input channel A and channel B
    int             m_A_pin, m_B_pin;

    // Determines which knob direction returns KNOB_LEFT vs. KNOB_RIGHT
    bool            m_orientation;
};
//=========================================================================================================

#endif
