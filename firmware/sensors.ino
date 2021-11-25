// Sensors functions for BORC 
// Updated 07/10/2021

// =================================================================
// Temp/RH reading
// =================================================================
void readTempRH()
{
  Serial.println("reading temp/rh");
  delay(5);
  // turn on sensor
  controlDevices(TEMP_SENSOR_POWER_PIN, HIGH);
  delay(10);
  
  // initialize sensor
  if(!sht31.begin(TEMP_SENSE_ADDRESS))
    errorCode |= (1<<TEMP_SENSE_ERR);
  else
    errorCode &= ~(1<<TEMP_SENSE_ERR);
  
  temp = sht31.readTemperature();    // in °C
  temp_f = (temp * 9.0/5.0) + 32.0;  // convert readings to °F
  rh = sht31.readHumidity();         // in %

  // turn off sensor
  controlDevices(TEMP_SENSOR_POWER_PIN, LOW);

  Serial.print("Temp: ");
  Serial.println(temp);
  Serial.print("RH: ");
  Serial.println(rh);
}

// =================================================================
// Current sensor reading
// =================================================================
void currentSense()
{
  // turn on sensor
  controlDevices(CURRENT_SENSE_POWER_PIN, HIGH);
  
  // initialize sensor
  if(!ina219.begin())
    errorCode |= (1<<CURRENT_SENSE_ERR);
  else
    errorCode &= ~(1<<CURRENT_SENSE_ERR);

  // get readings
  busVoltage = ina219.getBusVoltage_V();
  current = ina219.getCurrent_mA();

  // check if voltage readings are good
  if(busVoltage >= 3 || busVoltage <= 7)
    errorCode |= (1<<SERVO_POWER_ERR);
  else
    errorCode &= ~(1<<SERVO_POWER_ERR);

  // check if current readings are good
  if(current >= 3000 || current <= 0)
    errorCode |= (1<<SERVO_ERR);
  else
    errorCode &= ~(1<<SERVO_ERR);

  // turn off sensor
  controlDevices(CURRENT_SENSE_POWER_PIN, LOW);
}

// =================================================================
// Average current sensor reading
// =================================================================
float avgCurrentSense()
{
  float current_sum=0.0;
  float samples = 20.0;
  for (int i=0; i<samples; i++)
  {
    current = ina219.getCurrent_mA();
    current_sum += current;
  }
  return current_sum/samples;
}
