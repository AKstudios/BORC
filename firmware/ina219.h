//=========================================================================================================
// ina219.h - Defines a driver an INA219 current sensor via the I2C bus
//=========================================================================================================
#ifndef _INA219_H_
#define _INA219_H_
#include <stdint.h>

class CINA219
{
public:

    // Call this to initialize communication with the chip
    bool   init(unsigned char i2c_address);

    // Fetch the buss voltage in millivolts
    bool    get_bus_mv(int16_t* p_value);

    // Fetch the voltage across the shunt resistor, in millivolts
    bool    get_shunt_mv(int16_t* p_value);

    // Fetches the current across the shunt resistor, in milliamps
    bool    get_current_ma(int16_t* p_value);
    int16_t get_current_ma();

    // Call this to read a register from the chip
    bool    read(unsigned char index, int16_t* p_value = nullptr);

    // Call this to write a value to a register
    bool    write(unsigned char index, int16_t value);

protected:

    unsigned char m_i2c_address;
};

#endif