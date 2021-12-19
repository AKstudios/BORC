//=========================================================================================================
// temphum.cpp - SHT3x Temperature and humidity sensor
//=========================================================================================================
#include "globals.h"
#include <Adafruit_SHT31.h>           //https://github.com/adafruit/Adafruit_SHT31

// declare Adafruit's SHT31 class object
static Adafruit_SHT31 sht31;

//=========================================================================================================
// read_temp_c() - initialize the sensor and read temperature
//=========================================================================================================
float CTempHum::read_temp_c()
{   
    // If we're using temperature simulation, return the simulated temperature
    if (m_is_simulated) return m_simulated_temp_c;

    // intialize sensor
    sht31.begin(TEMP_SENSE_ADDRESS);

    // get temperature readings in °C
    float temp_c = sht31.readTemperature();    

    // if reading is NaN, try reading temp again
    if (isnan(temp_c)) temp_c = sht31.readTemperature();

    // send the temperature back to caller
    return temp_c;
}
//=========================================================================================================


//=========================================================================================================
// read_hum() - // initialize the sensor and read temperature and humidity 
//=========================================================================================================
float CTempHum::read_hum()
{  
    // intialize sensor
    sht31.begin(TEMP_SENSE_ADDRESS);

    // get humidity readings in %
    float rh = sht31.readHumidity();

    // send the rh % back to caller
    return rh;
}
//=========================================================================================================


//=========================================================================================================
// simulate_temp_c() - Tell the object to always report this simulated temperature
//=========================================================================================================
void CTempHum::simulate_temp_c(float temperature_c)
{
    // Store the simualate temperature for future use
    m_simulated_temp_c = temperature_c;

    // Any impossible temperature means "we're simulating temperatures"
    m_is_simulated = (temperature_c <= -99 || temperature_c >= 99);

}
//=========================================================================================================
