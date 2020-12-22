// Actions functions for BORC
// Updated 12/22/2020

// =================================================================
// Temp/RH reading
// =================================================================
void readTempRH()
{
  // initialize sensor
  if(!sht31.begin(0x44))
    errorCode |= (1<<TEMP_SENSE_ERR);
  else
    errorCode &= ~(1<<TEMP_SENSE_ERR);
  
  temp = sht31.readTemperature();    // in °C
  temp_f = (temp * 9.0/5.0) + 32.0;  // convert readings to °F
  rh = sht31.readHumidity();         // in %
}

// =================================================================
// Current sensor reading
// =================================================================
void currentSense()
{
  // enable current sensor
  if(!ina219.begin())
    errorCode |= (1<<CURRENT_SENSE_ERR);
  else
    errorCode &= ~(1<<CURRENT_SENSE_ERR);

  // get readings
  busVoltage = ina219.getBusVoltage_V();
  motorCurrent = ina219.getCurrent_mA();

  // check if voltage readings are good
  if(busVoltage >= 13 || busVoltage <= 11)
    errorCode |= (1<<MOTOR_POWER_ERR);
  else
    errorCode &= ~(1<<MOTOR_POWER_ERR);

  // check if current readings are good
  if(motorCurrent >= 1000 || motorCurrent <= 0)
    errorCode |= (1<<MOTOR_ERR);
  else
    errorCode &= ~(1<<MOTOR_ERR);
}

// =================================================================
// PID control calculations
// =================================================================


// =================================================================
// Adjust motor position
// =================================================================


// =================================================================
// Transmit data
// =================================================================

// =================================================================
