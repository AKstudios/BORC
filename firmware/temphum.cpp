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
float CTempHum::read()
{   
    // turn sensor's power on
    PowerMgr.powerOn(TEMP_SENSOR_POWER_PIN);

    // intialize sensor
    sht31.begin(TEMP_SENSE_ADDRESS);

    // get temperature readings in °C
    temp = sht31.readTemperature();    

    // get humidity readings in %
    rh = sht31.readHumidity();

    // send the temperature back to caller
    return temp;
}
//=========================================================================================================
