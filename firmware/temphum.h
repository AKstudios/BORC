#ifndef _TEMPHUM_H_
#define _TEMPHUM_H_

class CTempHum
{
public:
    // read the temperature and humidity
    // setting is_celcius to true returns temp in celsius, setting ot false returns temp in farenheit
    void read(bool is_celsius);

    // save temperature and humidity in these variables
    float temp, rh;
};

#endif
