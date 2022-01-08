//=========================================================================================================
// rotary_knob.cpp - Manages a list of events generated from a user rotating a knob left, rotating it
//                   right, or clicking on it (like a button)
//=========================================================================================================
#include "rotary_knob.h"
#include "common.h"
#include "Arduino.h"
#include "globals.h"

// Define a KNOB_CLICK event that is internal to this class and is never returned by get_event()
#define KNOB_CLICK ((knob_event_t)0x7F)


//=========================================================================================================
// ISRs for rotate and button-click
//=========================================================================================================
//ISR(INT1_vect)   { Knob.on_rotate_interrupt(); }
int1_isr()       { Knob.on_rotate_interrupt(); }
ISR(PCINT1_vect) { Knob.on_click_interrupt();  }
//=========================================================================================================



//=========================================================================================================
// on_click_interrupt() - The ISR for knob-clicks calls this to handle it
//=========================================================================================================
void CRotaryKnob::on_click_interrupt()
{
    start_debounce_timer(KNOB_CLICK);
}
//=========================================================================================================


//=========================================================================================================
// on_rotate_interrupt() - The ISR for rotation interrupts calls this to handle it
//=========================================================================================================
void CRotaryKnob::on_rotate_interrupt()
{
    knob_event_t event;
    
    // Read the rotary encoder B-pin
    int b_state = digitalRead(m_B_pin);

    // Determine if this is a rotate left or rotate right
    if (m_orientation)
        event = (b_state) ? KNOB_RIGHT : KNOB_LEFT;
    else
        event = (b_state) ? KNOB_LEFT :  KNOB_RIGHT;

    // account for debounce here
    Knob.start_debounce_timer(event);
}
//=========================================================================================================



//=========================================================================================================
// Debouce a specific event
//=========================================================================================================
void CRotaryKnob::start_debounce_timer(knob_event_t event)
{
  // Save the event being debounced
  m_debounce_event = event;

  // When this timer expires, our event has been debounced
  m_debounce_timer.start_from_isr();

  // Tell the sleep manager that there has been activity from the knob
  SleepMgr.on_knob_activity();
}
//=========================================================================================================


//=========================================================================================================
// configure_PCINT() - Configure the registers for the PCINT pin that the click-button is attached to
//=========================================================================================================
void CRotaryKnob::configure_PCINT(int pin)
{
    // This translates a the bank-number of a pin into a PCINT vector number
    unsigned char bank_to_pcint[] = { 1,3,2,0 };

    // Pins come in 4 banks of eight.  Which bank is this pin in?
    unsigned char bank = pin >> 3;

    // There are 4 sets of PCINT registers, numbered 0 thru 3.  Find out which one we're using
    unsigned char PCINT = bank_to_pcint[bank];

    // Find out which bit-number within the registers we need to twiddle
    unsigned char bit = pin & 7;

    // Create a convient mask
    unsigned char mask = (1 << bit);

    // Configure the registers for the appropriate pin-change interrupt
    switch (PCINT)
    {
    case 0:
        DDRA &= ~(mask);  // Make it an input
        PORTA  |= mask;   // Enable the pullup
        PCMSK0 |= mask;   // A change on this pin causes an interrupt
        break;

    case 1:
        DDRB &= ~(mask);  // Make it an input
        PORTB  |= mask;   // Enable the pullup
        PCMSK1 |= mask;   // A change on this pin causes an interrupt
        break;

    case 2:
        DDRC &= ~(mask);  // Make it an input
        PORTC  |= mask;   // Enable the pullup
        PCMSK2 |= mask;   // A change on this pin causes an interrupt
        break;

    #if 0   /* ---- This is commentd out so it will compile on an Atmega 2560 ---- */
    case 3:
        DDRD &= ~(mask);  // Make it an input
        PORTD  |= mask;   // Enable the pullup
        PCMSK3 |= mask;   // A change on this pin causes an interrupt
        break;
    #endif
    }

    // Clear any pending interrupt for this bank of pins
    PCIFR |= (1 << PCINT);

    // Configure "Pin Change Interrupt Control Register" to allow interrupts for this bank of pins
    PCICR |= (1 << PCINT);
}
//=========================================================================================================



//=========================================================================================================
// configure_INT() - Configure the registers for the INT pin that the rotary A-channel is attached to
//
// This routine examines the provided pin number, determines which INT pin (INT0, INT1, or INT2) that
// corresponds to, makes that pin an input, and enables interrupts for that INT
//=========================================================================================================
void CRotaryKnob::configure_INT(int pin, bool enable_pullup)
{
    int INT;

    // Determine which INT pin we are attached to
    switch (pin)
    {
        case 10:
            INT = 0;            // Pin INT0/PD2
            DDRD &= ~(1 << 2);  // Make the pin an input
            if (enable_pullup) PORTD |= (1 << 2);
            break;

        case 11:
            INT = 1;            // Pin INT1/PD3
            DDRD &= ~(1 << 3);  // Make the pin an input
            if (enable_pullup) PORTD |= (1 << 3);
            break;

        case 2:
            INT = 2;            // Pin INT2/PB2
            DDRB &= ~(1 << 2);  // Make the pin an input
            if (enable_pullup) PORTB |= (1 << 2);
            break;

        default:
            while (true);
    }

    // Configure the "External Interrupt Config Register A" to "We want an interrupt on rising edges"
    EICRA |= (3 << (INT << 1));

    // Configure the "External Interrupt Flag Register" to clear any pending interrupt from this INT pin
    EIFR  |= (1 << INT);

    // Configure the "External Interrupt Mask" to allow hardware interrupts on this INT pin
    EIMSK |= (1 << INT);
}
//=========================================================================================================



//=========================================================================================================
// init() - Called once to initialize this object
//
// This assumes rotary channel A is attached to an INT pin and that the pushbutton is attached to PCINT 
// 
// Passed:  A_pin    = The digital pin number for the rotary A channel
//          B_pin    = The digital pin number for the rotary B channel
//=========================================================================================================
void CRotaryKnob::init(int A_pin, int B_pin, int click_pin)
{   
    // Save the rotary A & B channel pin numbers for future reference
    m_A_pin = A_pin;
    m_B_pin = B_pin;

    // Need to make the rotary B-channel an input
    pinMode(m_B_pin, INPUT);

    // If we have physical hardware attached...
    if (!NO_HW)
    {
        // Configure the INT interrupt for the A-channel pin
        //configure_INT(A_pin, false);
        attachInterrupt(1, int1_isr, RISING);

        // Configure the PCINT interrupt for the click-pin
        configure_PCINT(click_pin);
    }

    // We haven't sensed any events yet
    m_event_count = 0;

    // The index of the next event to be added or retrieved starts at zero
    m_put_index = m_get_index = 0;

    // Tell the debounce timer what its settling time is
    m_debounce_timer.set_duration(50);

    // Set the knob left/right orientation
    m_orientation = true;
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
//             Also manages the sensing of KNOB_LPRESS events
//=========================================================================================================
void CRotaryKnob::execute()
{
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
            if (m_button_timer.is_running())
            {
                m_button_timer.stop();
                add_event(KNOB_UP);
            }
        }

        // If we get here, the button has just settled in the down position
        else m_button_timer.start(2000);
        
        // We're done debouncing the "KNOB_CLICK" activity
        return;
    }

    // Ensure that the sleep manager doesn't go to sleep while we're holding the button down
    if (m_button_timer.is_running()) SleepMgr.kick_sleep_timer();

    // If the button timer has expired then the button has been down for awhile
    if (m_button_timer.is_expired()) add_event(KNOB_LPRESS);
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