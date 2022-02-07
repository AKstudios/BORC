//=========================================================================================================
// misc_small.h - This file contains definitons for various small classes
//=========================================================================================================
#ifndef _MISC_SMALL_H_
#define _MISC_SMALL_H_


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                   USB CONNECTION SENSOR
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><


//=========================================================================================================
// CUSBSensor() - Senses when the USB is plugged or unplugged and reports change of state
//=========================================================================================================
class CUSBSensor
{
public:

    CUSBSensor() {m_state = 0xFF;}

    // State machine, call frequently
    void    execute();

    // Call this to find out if the USB is currently conected
    bool    current_state() {return m_state == 1;}

protected:

    // This is automatically called by execute when the state changes
    void    on_change_of_state();

    // 0 = Not plugged in, 1 = Plugged in, 0xFF = Unknown
    unsigned char m_state;
};
//=========================================================================================================


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                     Current Logger
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><


//=========================================================================================================
// CCurrentLogger() - Logs motor current
//=========================================================================================================
class CCurrentLogger
{
public:

    // Constructor - Disables logging
    CCurrentLogger() {m_is_enabled = false;}

    // Call this to start logging.  Passing a non-zero "seconds" automatically turns off the logger
    // after the specified number of seconds have elapsed
    void    start(uint16_t seconds = 0);
    
    // Call this to stop logging current
    void    stop();

    // Call this every time you want current-logging to happen
    void    execute(int current = READ);

protected:

    enum {READ = -9999};

    // A repeating timer of 1 second duration
    msTimer     m_second_timer;

    // Counts the number of elapsed seconds
    uint16_t    m_second_counter;

    // If this is true, current-logging is enabled
    bool        m_is_enabled;

    // Duration, in seconds that the logger should remain enabled
    uint16_t    m_duration;


};
//=========================================================================================================


//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//                                           BATTERY SENSOR
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><
//<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><


//=========================================================================================================
// CBattSensor() - checks and reports the battery status
//=========================================================================================================
class CBattSensor
{
public:
    // Call this to find out the current battery voltage in mV
    int     read_voltage();

    // this will call read_voltage and return true if the battery voltage is under acceptable threshold
    bool    is_low();
};
//=========================================================================================================

#endif