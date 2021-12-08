//=========================================================================================================
// rotary_knob.cpp - Manages a list of events generated from a user rotating a knob left, rotating it
//                   right, or clicking on it (like a button)
//=========================================================================================================
#include "rotary_knob.h"
#include "common.h"
#include "Arduino.h"
#include "globals.h"


//=========================================================================================================
// ISR for rotary knob rotation - this ISR is called on changing states of channel A pin
//=========================================================================================================
void knob_rotate_isr()
{   
    // this maintains the last known state of channel A
    static bool last_A_state = true;

    // read current state of channel A
    bool current_A_state = digitalRead(CHANNEL_A);

    // if this is rising edge of channel A
    if (current_A_state && !last_A_state)
    {
      // Determine if this is a rotate left or rotate right
      knob_event_t knob_event = digitalRead(CHANNEL_B)? KNOB_LEFT : KNOB_RIGHT;

      // account for debounce here
      Knob.start_debounce_timer(knob_event);
    }

    // save the last state of channel A
    last_A_state = current_A_state;
}
//=========================================================================================================

//=========================================================================================================
// ISR for rotary knob click - this ISR is called on changing states of click pin on rotary encoder
//=========================================================================================================
void knob_click_isr()
{
    Knob.start_debounce_timer(KNOB_CLICK);
}

ISR(PCINT1_vect) {knob_click_isr();}
//=========================================================================================================


//=========================================================================================================
// Debouce a specific event
//=========================================================================================================
void CRotaryKnob::start_debounce_timer(knob_event_t event)
{
  // save the event being debounced
  m_debounce_event = event;

  // When this timer expires, our event has been debounced
  m_debounce_timer.start(50);

  // tell the sleep manager we want to go to sleep 5 seconds from now
  SleepMgr.kick_timer();

  // tell the manager we're awake by knob
  SleepMgr.signal_wakeup();
}
//=========================================================================================================


//=========================================================================================================
// init() - Called once to initialize this object
//=========================================================================================================
void CRotaryKnob::init()
{   
    // Input pins of the rotary encoder
    pinMode(CHANNEL_A, INPUT);
    pinMode(CHANNEL_B, INPUT);
      
    // Enable ISR on channel A to interrupt at changing edges
    attachInterrupt(0, knob_rotate_isr, CHANGE);

    // enable pin-change interrupt for knob clicks
    cli();  // disable global interrupts
    PORTB |= (1<<PORTB0); //Activate pullup on pin PB0
    PCIFR |= (1<<PCIF1); // clear outstanding interrupt on PCINT[15:8] 
    PCICR |= (1<<PCIE1); // enable interrupts on PCINT[15:8]
    PCMSK1 |= (1<<PCINT8); // pin change mask register for pin D0 (PCINT8)
    sei();  // enable global interrupts

    // We haven't sensed any events yet
    m_event_count = 0;

    // The index of the next event to be added or retrieved starts at zero
    m_put_index = m_get_index = 0;

}
//=========================================================================================================


//=========================================================================================================
// add_event() - The execute() routine calls this to add a new event to the queue of pending events
//=========================================================================================================
void CRotaryKnob::add_event(knob_event_t event)
{
    // If there is room in the event queue...
    if (m_event_count < MAX_EVENTS && !m_throw_away_next_event) 
    {
        // Stuff this event into our circular queue
        m_event[m_put_index++] = event;

        // If our index has fallen off the end of our queue, wrap it around
        if (m_put_index >= MAX_EVENTS) m_put_index = 0;

        // We now have one more event in our queue
        ++m_event_count;
    }

    // This flag is only true for one call to this routine
    m_throw_away_next_event = false;

}
//=========================================================================================================


//=========================================================================================================
// execute() - Checks for debounced events and adds them to the event queue.
//             Also managed the sensing of KNOB_LPRESS events
//=========================================================================================================
void CRotaryKnob::execute()
{
    static OneShot button_timer;

    // If the knob has just settled into a fully debounced position...
    if (m_debounce_timer.is_expired())
    {
        // If we just debounced a KNOB_LEFT action, add the event
        if (m_debounce_event == KNOB_LEFT)
        {
            add_event(KNOB_LEFT);
            return;
        }
                
        
        // If we just debounced a KNOB_RIGHT action, add the event
        if (m_debounce_event == KNOB_RIGHT)
        {
            add_event(KNOB_RIGHT);
            return;
        }

        // If we get here, we assume we're handling a KNOB_CLICK action
        bool button_is_up = digitalRead(CLICK_PIN) != 0;

        // If the button has just settled in the "up" position and the 
        // timer says the "button down timer" isn't expired, we can
        // assume this was a short press of the button
        if (button_is_up)
        {
            if (button_timer.is_running() && !button_timer.is_expired())
            {
                button_timer.stop();
                add_event(KNOB_UP);
            }
            return;
        }

        // If we get here, the button has just settled in the down position
        button_timer.start(2000);
    }

    // If the button timer has expired then the button has been down for awhile
    if (button_timer.is_expired()) add_event(KNOB_LPRESS);
}
//=========================================================================================================




//=========================================================================================================
// get_event() - If there is an unhandled event pending, this fetches the event
//
// Returns:  true  = There is an event pending (stored in *p_event)
//           false = There is no event pending
//=========================================================================================================
bool CRotaryKnob::get_event(knob_event_t *p_event)
{
    // Give the debouncer a chance to add events to the queue
    execute();

    // If there are no pending events waiting to be handled, we're done
    if (m_event_count == 0) return false;

    // Fill in the caller's field with the next pending event
    *p_event = m_event[m_get_index++];

    // If our index has fallen off the end of our queue, wrap it around
    if (m_get_index >= MAX_EVENTS) m_get_index = 0;

    // We now have one fewer event in the queue
    --m_event_count;

    // Tell the caller that they have an event to handle!
    return true;
}
//=========================================================================================================