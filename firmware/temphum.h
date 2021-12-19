#ifndef _TEMPHUM_H_
#define _TEMPHUM_H_

class CTempHum
{
public:

    // Constructor
    CTempHum() {m_is_simulated = false;}

    // read the temperature and return in celsius
    float   read_temp_c();

    // read humidity and return in %
    float   read_hum();

    // Call this to simulate room temperatures
    void    simulate_temp_c(float temperature);

protected:

    bool    m_is_simulated;

    float   m_simulated_temp_c;

};

#endif
