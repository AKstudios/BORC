#ifndef _TEMPHUM_H_
#define _TEMPHUM_H_

class CTempHum
{
public:
    // read the temperature and return in celsius
    float read_temp();

    // read humidity and return in %
    float read_hum();

    // save temperature and humidity in these variables
    float temp, rh;
};

#endif
