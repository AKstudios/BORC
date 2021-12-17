#ifndef _TEMPHUM_H_
#define _TEMPHUM_H_

class CTempHum
{
public:
    // read the temperature and humidity
    // returns temperature in celsius
    float read();

    // save temperature and humidity in these variables
    float temp, rh;
};

#endif
