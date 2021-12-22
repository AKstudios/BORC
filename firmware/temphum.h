#ifndef _TEMPHUM_H_
#define _TEMPHUM_H_

class CTempHum
{
public:

    // Constructor
    CTempHum() {m_is_simulated = false;}

    // read humidity and return in %
    float   read_hum();

    // Return the temperature in F
    float   read_temp_f();

    // Call this to simulate room temperatures
    void    simulate_temp_f(float temperature);

protected:

    // read the temperature and return in celsius
    float   read_temp_c();

    bool    m_is_simulated;

    float   m_simulated_temp_f;

};

#endif
