//=========================================================================================================
// serial_logger.cpp - Defines a serial data-logger
//=========================================================================================================
#include "globals.h"


//=========================================================================================================
// These <MUST> come in the same order as the data_item_t enum!!
//=========================================================================================================
static const char* data_name[] =
{
    "DATA",
    "TIME_UNIT",
    "TEMP_F",
    "SETPOINT_F",
    "CURRENT"
};
//=========================================================================================================


//=========================================================================================================
// Init() - Called once at startup
//=========================================================================================================
void CSerialLogger::init()
{
    set_time_unit(TU_MILLIS);
}
//=========================================================================================================


//=========================================================================================================
// log() - Logs a line of data to the serial port
//=========================================================================================================
void CSerialLogger::log(data_item_t item, uint32_t value)
{
    char buffer[100], *out = buffer;

    // If the USB serial port isn't plugged in, do nothing
    if (!USB.current_state()) return;

    // How much time has elapsed since we last started the clock?
    uint32_t now = (m_time_unit == TU_MILLIS ? millis() : micros()) - m_clock_start;

    // Format our output
    out += snprintf(out, sizeof(buffer), "$:,%10lu", now);
    out += snprintf(out, sizeof(buffer), ", %10s", data_name[item]);
    out += snprintf(out, sizeof(buffer), ", %u", value);

    // Write our log line to the serial output buffer
    Serial.println(buffer);
}
//=========================================================================================================


//=========================================================================================================
// log() - Logs a line of data to the serial port
//=========================================================================================================
void CSerialLogger::log(data_item_t item, float& value)
{
    char buffer[100], *out = buffer;

    // If the USB serial port isn't plugged in, do nothing
    if (!USB.current_state()) return;

    // How much time has elapsed since we last started the clock?
    uint32_t now = (m_time_unit == TU_MILLIS ? millis() : micros()) - m_clock_start;

    // Format our output
    out += snprintf(out, sizeof(buffer), "$:,%10d", now);
    out += snprintf(out, sizeof(buffer), ", %10s", data_name[item]);
    out += snprintf(out, sizeof(buffer), ", %s", strfloat(value, 0, 2));

    // Write our log line to the serial output buffer
    Serial.println(buffer);
}
//=========================================================================================================



//=========================================================================================================
// set_time_unit() - Determine what the time unit for logging is
//=========================================================================================================
void CSerialLogger::set_time_unit(time_unit_t unit)
{
    m_time_unit = unit;
    m_clock_start = (unit == TU_MILLIS) ? millis() : micros();
    log(DI_TIME_UNIT, unit);
}
//=========================================================================================================


//=========================================================================================================
// push_time_unit() - Saves the current time unit and starts a new one
//=========================================================================================================
void CSerialLogger::push_time_unit(time_unit_t new_unit)
{
    // Push the current unit onto the time_unit stack
    m_time_unit_stack = (m_time_unit_stack << 1) | m_time_unit;

    // And start using the new time unit
    set_time_unit(new_unit);
}
//=========================================================================================================



//=========================================================================================================
// pop_time_unit() - Restores the most recently pushed time unit
//=========================================================================================================
void CSerialLogger::pop_time_unit()
{
    // Fetch the most recently pushed time unit
    time_unit_t new_unit = (time_unit_t)(m_time_unit_stack & 1);

    // Pop it off the stack
    m_time_unit_stack >>= 1;

    // And start using the new time unit
    set_time_unit(new_unit);
}
//=========================================================================================================








