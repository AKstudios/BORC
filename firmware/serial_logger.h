
//=========================================================================================================
// serial_logger.h - Defines a serial data-logger
//=========================================================================================================
#ifndef _SERIAL_LOGGER_H_
#define _SERIAL_LOGGER_H_
#include <stdint.h>


//=========================================================================================================
// These <MUST> come in the same order as the data_name[] array in the .cpp file!!
//=========================================================================================================
enum data_item_t
{
    DI_DATA = 0,
    DI_TIME_UNIT,
    DI_TEMP_F,
    DI_SETPOINT_F,
    DI_CURRENT
};
//=========================================================================================================

// These are for specifying time-units in set_time_unit() and push_time_unit()
enum time_unit_t { TU_MILLIS = 0, TU_MICROS = 1};

class CSerialLogger
{
public:

    // Call this once at startup
    void    init();

    // Call these to log data items
    void    log(data_item_t item, uint32_t value);
    void    log(data_item_t item, float&   value);

    // Call this to set a new time unit (TU_MILLIS or TU_MICROS)
    void    set_time_unit(time_unit_t time_unit);

    // Call this to save the current time unit and set a new one
    void    push_time_unit(time_unit_t new_unit);

    // Restores the most recently pushed time unit
    void    pop_time_unit();

protected:
  
    uint8_t     m_time_unit;
    uint16_t    m_time_unit_stack;
    uint32_t    m_clock_start;
};
#endif


