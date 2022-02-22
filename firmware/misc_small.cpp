//=========================================================================================================
// misc_small.h - This file contains implementations for various small classes
//=========================================================================================================
#include "globals.h"

//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                   USB CONNECTION SENSOR
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><



//=========================================================================================================
// execute() - State machine used to sense and report changes of state in the USB connection
//=========================================================================================================
void CUSBSensor::execute()
{
    // Find out of the USB is currently plugged in
    unsigned char is_connected = (analogRead(VBUS_SENSE) > 400) ? 1:0;    

    // If we don't have hardware, we're always in USB mode
    if (NO_HW) is_connected = 1;

    // If this state is different than the last sensed state...
    if (is_connected != m_state)
    {
        m_state = is_connected;
        on_change_of_state();        
    }
}
//=========================================================================================================


//=========================================================================================================
// on_change_of_state() - Called anytime the state of the USB connection changes
//=========================================================================================================
void CUSBSensor::on_change_of_state()
{
    if (m_state)
        SleepMgr.start_awake_mode();
    else
        SleepMgr.start_sleep_mode();
}
//=========================================================================================================



//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                    Current Logger
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><


//=========================================================================================================
// start() - Enables current logging
//=========================================================================================================
void CCurrentLogger::start(uint16_t duration_seconds = 0)
{
    // Current logging is now enabled
    m_is_enabled = true;

    // If logging should automatically shut-off after a specified number of seconds...
    if (duration_seconds)
    {
        // Keep track of how many seconds should elapsed before auto-shutoff
        m_duration = duration_seconds;
        
        // Zero seconds have elapsed
        m_second_counter = 0;

        // Start a repeating timer that expires once a second
        m_second_timer.start(1000);
    }
}
//=========================================================================================================


//=========================================================================================================
// stop() - Stops current logging
//=========================================================================================================
void CCurrentLogger::stop()
{
    // We're no longer logging current
    m_is_enabled = false;    
}
//=========================================================================================================


//=========================================================================================================
// execute() - Logs the current and also manages the timer
//=========================================================================================================
void CCurrentLogger::execute(int current)
{
    // If logging isn't enabled, do nothing
    if (!m_is_enabled) return;

    // If the auto-shutoff time has elapsed, disable logging
    if (m_second_timer.is_expired() && ++m_second_counter >= m_duration)
    {
        m_is_enabled = false;
        return;
    }
    
    // Fetch the current in mA
    if (current == READ) current = INA219.get_current_ma();

    // Log the current in mA
    Logger.log(DI_CURRENT, current);
}
//=========================================================================================================


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                           BATTERY SENSOR
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><



//=========================================================================================================
// read_voltage() - Call this to find out the current battery voltage in mV
//=========================================================================================================
int CBattSensor::read_voltage()
{
    // Read the raw ADC reading of battery
    unsigned long adc_reading = analogRead(VBAT_SENSE);

    // .. and convert it into a voltage reading
    // (assuming system voltage is 3.3V and voltage divider splits input voltage in half)
    int voltage_mv = (adc_reading * 2 * 3300) / 1023;

    // tell the caller the battery voltage
    return voltage_mv;
}
//=========================================================================================================


//=========================================================================================================
// is_low() - return true if the battery voltage is under acceptable threshold
//=========================================================================================================
bool CBattSensor::is_low()
{   
    // check if voltage is under threshold and return status
    if (read_voltage() <= 3650)
    {
        System.error_byte |= BATT_ERR;
        return true;
    }
    else
    {
        System.error_byte &= ~BATT_ERR;
        return false;
    }
}
//=========================================================================================================