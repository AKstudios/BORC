//=========================================================================================================
// temphum.cpp - SHT3x Temperature and humidity sensor
//=========================================================================================================
#include "globals.h"
#include <Adafruit_SHT31.h>           //https://github.com/adafruit/Adafruit_SHT31

// declare Adafruit's SHT31 class object
static Adafruit_SHT31 sht31;

//=========================================================================================================
// read_temp() - // initialize the sensor and read temperature
//=========================================================================================================
float CTempHum::read_temp()
{   
    // intialize sensor
    sht31.begin(TEMP_SENSE_ADDRESS);

    // get temperature readings in °C
    temp = sht31.readTemperature();    

    // if reading is NaN, try reading temp again
    if (isnan(temp))    temp = sht31.readTemperature();

    // send the temperature back to caller
    return temp;
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
    rh = sht31.readHumidity();

    // send the rh % back to caller
    return rh;
}
//=========================================================================================================
