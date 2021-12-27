//=========================================================================================================
// ina219.cpp - Implements a driver an INA219 current sensor via the I2C bus
//=========================================================================================================
#include "ina219.h"
#include "i2c.h"


#define CONFIG_REGISTER        0
#define SHUNT_VOLTAGE_REGISTER 1
#define BUS_VOLTAGE_REGISTER   2
#define CURRENT_REGISTER       4
#define CALIBRATION_REGISTER   5

//=========================================================================================================
// These bits
//=========================================================================================================
#define MODE_AVG_MASK ~(0x0F << 3)
#define MODE_AVG_2     (0x09 << 3)
#define MODE_AVG_4     (0x0A << 3)
#define MODE_AVG_8     (0x0B << 3)
#define MODE_AVG_16    (0x0C << 3)
#define MODE_AVG_32    (0x0D << 3)
#define MODE_AVG_64    (0x0E << 3)
#define MODE_AVG_128   (0x0F << 3)


//=========================================================================================================
// We assume a shunt-resistor value of 0.1 ohms and a desired measurement resolution of .0001 amps.
//
// The formula for computing the calibration value is:
//
// Calibration constant = 0.04096 / (desired_resolution * shunt_resistor_value)
//
// .04096 / (0.0001 * 0.1) = 4096
//=========================================================================================================
#define CAL_CONSTANT 4096
//=========================================================================================================


//=========================================================================================================
// init() - Call this once at bootup initialize this device
//=========================================================================================================
bool CINA219::init(unsigned char i2c_address)
{
    int16_t config;

    // Save our I2C address for future use
    m_i2c_address = i2c_address;

    // This will cause the INA219 to reset to its default settings
    if (!write(CONFIG_REGISTER, 0x8000)) return false;

    // And tell the caller whether we are able to write the calibration constant to the register
    return write(CALIBRATION_REGISTER, CAL_CONSTANT);
}
//=========================================================================================================


//=========================================================================================================
// read() - Reads and optionally returns the value in a register.  Valid registers indices are 0 thru 5
//=========================================================================================================
bool CINA219::read(unsigned char index, int16_t* p_value)
{
    unsigned char data[2] = {index};

    // Write the address of the register that we intend to read
    if (twi_writeTo(m_i2c_address, data, 1, true, true) != 0)
    {
        if (p_value) *p_value = 0;
        return false;
    }

    // Now read the 2-byte register value
    if (twi_readFrom(m_i2c_address, data, 2, true) != 2)
    {
        if (p_value) *p_value = 0;
        return false;
    }

    // If the caller wants to see the data, pack it into an integer and hand it to him
    if (p_value) *p_value = (data[0] << 8) | data[1];

    // Tell the caller this operation was successful
    return true;
}
//=========================================================================================================


//=========================================================================================================
// write() - Call this to write a value to a register.  Only register indices 0 and 5 are valid
//=========================================================================================================
bool CINA219::write(unsigned char index, int16_t value)
{
    // We're going to send three bytes: the index, the MSB of the data, and the LSB of the data
    unsigned char data[3] = {index, value >> 8, value & 0xFF};

    // Write the specified value to the specified register
    bool status = (twi_writeTo(m_i2c_address, data, 3, true, true) == 0);

    // Tell the caller whether or not this worked
    return status;
}
//=========================================================================================================



//=========================================================================================================
// get_bus_mv() - Fetches the bus voltage in millivolts
//=========================================================================================================
bool CINA219::get_bus_mv(int16_t* p_value)
{
    int adc;

    // Read the bus voltage register
    if (!read(BUS_VOLTAGE_REGISTER, &adc))
    {
        *p_value = 0;
        return false;
    }

    // Right justify the ADC value
    adc >>= 3;

    // Each ADC count corresponds to 4 mv
    *p_value = adc << 2;

    // Tell the caller that he has a voltage
    return true;
}
//=========================================================================================================


//=========================================================================================================
// get_shunt_mv() - Fetches the shunt voltage in millivolts
//=========================================================================================================
bool CINA219::get_shunt_mv(int16_t* p_value)
{
    int16_t adc;

    // Read the bus voltage register
    if (!read(SHUNT_VOLTAGE_REGISTER, &adc))
    {
        *p_value = 0;
        return false;
    }

    // The ADC value is in 10uV units, so divide by 100 to convert to millivolts
    adc /= 100;

    // If the user wants the result value, store it for him
    *p_value = adc;    

    // Tell the caller that he has a voltage
    return true;
}
//=========================================================================================================



//=========================================================================================================
// get_current_ma() - Fetches the most recent current measurment in milliamps
//=========================================================================================================
int16_t CINA219::get_current_ma()
{
    int16_t current;
    get_current_ma(&current);
    return current;    
}
bool CINA219::get_current_ma(int16_t* p_value)
{
    int16_t adc;

    // Just in case the chip reset from a power spike, restore our calibration constant
    if (!write(CALIBRATION_REGISTER, CAL_CONSTANT))
    {
        *p_value = 0;
        return false;
    }

    // Read the current register
    if (!read(CURRENT_REGISTER, &adc))
    {
        *p_value = 0;
        return false;
    }

    // We have the calibration set such that an ADC count 10 of a mA.  Convert to mA
    adc /= 10;

    // If the user wants the result value, store it for him
    *p_value = adc;    

    // Tell the caller that all is well
    return true;
}
//=========================================================================================================


