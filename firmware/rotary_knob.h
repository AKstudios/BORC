#ifndef _ROTARY_KNOB_H
#define _ROTARY_KNOB_H

#include "oneshot.h"

//=========================================================================================================
// These are the types of events a user can generate by manipulating the rotary knob
//=========================================================================================================
enum knob_event_t : char
{
  KNOB_LEFT,
  KNOB_RIGHT,
  KNOB_CLICK
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
    void            init(); 

    // Fetches the next available knob event.  Call this often enough to keep up with events
    bool            get_event(knob_event_t* p_event);  

    // Returns state of the one shot timer
    bool            is_inactive() {return m_activity_timer.is_expired();}

protected:

    // The ISR is lonely, it needs access to add event, so make it a friend
    friend void knob_rotate_isr();

    // The ISR is lonely, it needs access to add event, so make it a friend
    friend void knob_click_isr();

    // start deboucing an event
    void start_debounce_timer(knob_event_t event);

    // We will keep a queue of a maximum of four pending events
    enum {MAX_EVENTS = 4};

    // Our ISR calls this to add an event to the list of pending events
    void            add_event(knob_event_t event);

    // The button ISR starts this timer every time is senses a button press
    OneShot         m_debounce_timer;

    // This timer expires when the knob has not been touched for a certain amount of time
    OneShot         m_activity_timer;

    // This timer expires when the knob is being pressed and held for a certain amount of time
    OneShot         m_long_press_timer;

    // How many pending events are there?
    char            m_event_count;

    // The list of pending knob events
    knob_event_t    m_event[MAX_EVENTS];

    // debounce event
    knob_event_t    m_debounce_event;

    // Index where the next event will be added
    int             m_put_index;

    // Index where the next event will be fetched from
    int             m_get_index;

};
//=========================================================================================================

#endif
