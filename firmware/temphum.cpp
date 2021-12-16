//=========================================================================================================
// temphum.cpp - SHT3x Temperature and humidity sensor
//=========================================================================================================
#include "globals.h"
#include <Adafruit_SHT31.h>           //https://github.com/adafruit/Adafruit_SHT31

// declare Adafruit's SHT31 class object
static Adafruit_SHT31 sht31;

//=========================================================================================================
// read() - // initialize the sensor and read temperature and humidity 
//=========================================================================================================
void CTempHum::read(bool is_celsius)
{
    // turn sensor's power on
    PowerMgr.powerOn(TEMP_SENSOR_POWER_PIN);
    
    // intialize sensor
    sht31.begin(TEMP_SENSE_ADDRESS);

    // get temperature readings in °C
    temp = sht31.readTemperature();    

    // convert readings to °F if requested
    if (!is_celsius)    temp = (temp * 9.0/5.0) + 32.0;

    // get humidity readings in %
    rh = sht31.readHumidity();
}
//=========================================================================================================
