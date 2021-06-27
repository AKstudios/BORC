// Actions functions for BORC (read sensors, debug)
// Updated 04/11/2021

// =================================================================
// System reset in extreme situations if code gets hanged somewhere
// =================================================================
void systemReset()
{
  // clear configuration settings in EEPROM
  for (int i = 0 ; i < 500 ; i++) {
    EEPROM.write(i, 0);
  }

  // reset system
  asm volatile ("jmp 0");
}

// =================================================================
// Control devices (turn on/off)
// =================================================================
void controlDevices(uint8_t pin, byte action)
{
  if (pin == 99)  // set all devices
  {
    if (action == 1)  // turn on devices
    {
      // set pin modes for devices as OUTPUT
      pinMode(CURRENT_SENSE_POWER_PIN, OUTPUT);
//      pinMode(SERVO_POWER_PIN, OUTPUT);
      pinMode(TEMP_SENSOR_POWER_PIN, OUTPUT);
      pinMode(LED_SCREEN_POWER_PIN, OUTPUT);
      pinMode(DRIVER_POWER_PIN, OUTPUT);
    
      // turn on all devices
      digitalWrite(CURRENT_SENSE_POWER_PIN, HIGH);
      digitalWrite(LED_SCREEN_POWER_PIN, HIGH);
//      digitalWrite(SERVO_POWER_PIN, HIGH);
      digitalWrite(TEMP_SENSOR_POWER_PIN, HIGH);
      digitalWrite(DRIVER_POWER_PIN, HIGH);
    }
    else if (action == 0) // turn off devices
    {
      // turn off all devices
      digitalWrite(CURRENT_SENSE_POWER_PIN, LOW);
      digitalWrite(LED_SCREEN_POWER_PIN, LOW);
      digitalWrite(SERVO_POWER_PIN, LOW);
      digitalWrite(TEMP_SENSOR_POWER_PIN, LOW);
      digitalWrite(DRIVER_POWER_PIN, LOW);
      
      // set pin modes for devices as INPUT
      pinMode(DRIVER_POWER_PIN, INPUT);
      pinMode(LED_SCREEN_POWER_PIN, INPUT);
      pinMode(CURRENT_SENSE_POWER_PIN, INPUT);
      pinMode(SERVO_POWER_PIN, INPUT);
      pinMode(TEMP_SENSOR_POWER_PIN, INPUT);
    }
  }

  else  // set a specific device
  {
    if (action == 1)
    {
      pinMode(pin, OUTPUT);
      digitalWrite(pin, HIGH);
    }
    else if (action == 0)
    {
      digitalWrite(pin, LOW);
      pinMode(pin, INPUT);
    }
  }
  delay(1);
}

// =================================================================
// Enable knob interrupts
// =================================================================
void enableKnobInterrupts()
{
  attachInterrupt(0, encoderISR, CHANGE);
  attachInterrupt(1, encoderISR, CHANGE);
}

// =================================================================
// Disable knob interrupts
// =================================================================
void disableKnobInterrupts()
{
  detachInterrupt(0);
  detachInterrupt(1);
}

// =================================================================
// Temp/RH reading
// =================================================================
void readTempRH()
{
  // turn on sensor
  controlDevices(TEMP_SENSOR_POWER_PIN, HIGH);
  
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

// =================================================================
// Check error code
// =================================================================
void debug()
{
  checkBattery(VBAT_SENSE);
  if(errorCode != 0 && errorCode != 0x80)  // errors detected apart from battery
    blinkLED(1,0,1,10);       // blink purple to indicate error
  else if(errorCode == 0x80)  // battery low
    blinkLED(1,0,0,10);       // blink red to indicate battery low
  else if(errorCode == 0)     // no errors
    blinkLED(0,0,0,0);        // don't blink LED
  Serial.print("Debug code: ");
  Serial.println(errorCode, HEX);
  delay(5);
}

// =================================================================
// Check battery level
// =================================================================
void checkBattery(int pin)
{
  float averageADC = readADC(pin, 5);
  batteryVoltage = (averageADC/1023) * 2 * 3.3;  // convert ADC to voltage reading
  if(batteryVoltage < 3.6)    // battery below 3.6 is low or has problem
    errorCode |= (1<<BATT_ERR);
  else if(batteryVoltage >= 3.6 && batteryVoltage <= 4.3)   // battery is nominal
    errorCode &= ~(1<<BATT_ERR);
}

// =================================================================
// Get average ADC for an analog pin with n samples
// =================================================================
float readADC(int pin, int n)
{
  float sum=0.0;
  for(int i=0;i<n;i++)
  {
     sum = sum + analogRead(pin);
  }
  float average = sum/float(n);
  return average;
}

// =================================================================
// Parse incoming data
// =================================================================
boolean parseData()
{
  // First identify data type
  char dataType[1];
  char * strtokIndx; // this is used by strtok() as an index
  char _dataReceived[100];

  strcpy(_dataReceived, dataReceived); // copy array into an expendible one

  strtokIndx = strtok(_dataReceived,","); // get the first part - the string
  strcpy(dataType, strtokIndx); // copy it to dataType

  Serial.println(dataType[0]);

  // Validate integrity of data packet before proceeding further
  if(validateDataPacket(dataType)==false)
  {
    Serial.println("cant validate data packet fail");
    return false;
  }

  // Continue with parsing
  if(dataType[0] == 'c')
  {
    Serial.println("config packet detected");
    strtokIndx = strtok(NULL, ","); // this continues where the previous strtok call left off
    strcpy(_UID, strtokIndx);
    strtokIndx = strtok(NULL, ",");
    strcpy(_NODEID, strtokIndx);
    strtokIndx = strtok(NULL, ",");
    strcpy(_NETWORKID, strtokIndx);
    strtokIndx = strtok(NULL, ",");
    strcpy(_ENCRYPTKEY, strtokIndx);

    Serial.println(_UID);
    Serial.println(_NODEID);
    Serial.println(_NETWORKID);
    Serial.println(_ENCRYPTKEY);

    Serial.println(sizeof(_UID));
    Serial.println(sizeof(_NODEID));
    Serial.println(sizeof(_NETWORKID));
    Serial.println(sizeof(_ENCRYPTKEY));
  }

  else if(dataType[0] == 'u')
  {
    // nothing more to do here
  }

  else if(dataType[0] == 's')
  {
    strtokIndx = strtok(NULL, ",");
    strcpy(_setpoint, strtokIndx);
  }

  else if(dataType[0] == 'i')
  {
    strtokIndx = strtok(NULL, ",");
    strcpy(_interval, strtokIndx);
  }

  else
    return false;

  // return true after successful parsing
  if(validateData(dataType)==true)
  {
    Serial.println("validate data ok");
    return true;
  }
  else
  {
    Serial.println("validate data fail");
    return false;
  }
    
}

// =================================================================
// Validate incoming data - return true if data looks good, false if bad
// =================================================================
boolean validateDataPacket(char _dataType[1])
{
  Serial.println("validating data packet..");

  byte commaCount = countOccurence(dataReceived, sizeof(dataReceived), ','); // count number of commas in the datapacket
  int dataReceivedSize = getCharArraySize(dataReceived, sizeof(dataReceived));
  
  Serial.print("No. of commas: ");
  Serial.println(commaCount);
  Serial.print("Size of full packet: ");
  Serial.println(sizeof(dataReceived));
  Serial.print("actual data packet size: ");
  Serial.println(dataReceivedSize);
  
  if(_dataType[0] == 'c')  // config
  {
    Serial.println("config packet detected");
    if(commaCount!=4)  // config packet will always have 4 commas
    {
      Serial.println("not 4 commas");
      return false;
    }
      
    if(dataReceivedSize >= 39 && dataReceivedSize <= 44)  // config packet will always be between 39 and 44 bytes
    {  Serial.println("size ok - between 39 and 44");return true;}
    else
      {Serial.println("size not ok");return false;}
  }

  else if(_dataType[0] == 'u')   // firmware update
  {
    if(commaCount!=0)  // update packet will always have 0 commas
      return false;
    if(dataReceivedSize == 1)  // update packet will always be 1 byte
      return true;
    else
      return false;
  }

  else if(_dataType[0] == 's')   // setpoint
  {
    if(commaCount!=1)  // config packet will always have 1 comma
      return false;
    if(dataReceivedSize >= 3 && dataReceivedSize <= 5)  // setpoint packet will always be between 3 and 5 bytes
      return true;
    else
      return false;
  }

  else if(_dataType[0] == 'i')   // interval
  {
    if(commaCount!=1)  // interval packet will always have 1 comma
      return false;
    if(dataReceivedSize >= 3 && dataReceivedSize <= 7)  // interval packet will always be between 3 and 7 bytes
      return true;
    else
      return false;
  }

  else    // invalid data type
    {Serial.println("datatype wrong");return false;}
}

// =================================================================
// Validate variables in datapacket - return true if good, false if bad
// =================================================================
boolean validateData(char _dataType[1])
{
  Serial.println("validating actual data");
  if(_dataType[0] == 'c')
  {
    Serial.println("validating config type");
    // check UID
    if(memcmp(_UID, UID, sizeof(_UID)) != 0)  // check if data was meant for this node
      {Serial.println("UID not same");return false;}

    // check NODEID
    int _NODEIDSIZE = getCharArraySize(_NODEID, sizeof(_NODEID));
    if(_NODEIDSIZE > 0 && _NODEIDSIZE <= 4)
    {
      Serial.println("node id size ok");
      uint16_t id = atoi(_NODEID);  // convert char array to int
      if(id > 0 && id <= 1023)
      {
        Serial.println("assigning new node id");
        NODEID = id;
      }
      else
        {Serial.println("node id out of bounds");return false;}
    }
    else
      {Serial.println("node id size is wrong");return false;}

    // check NETWORKID
    int _NETWORKIDSIZE = getCharArraySize(_NETWORKID, sizeof(_NETWORKID));
    if(_NETWORKIDSIZE > 0 && _NETWORKIDSIZE <= 3)
    {
      Serial.println("network id size ok");
      uint8_t nid = atoi(_NETWORKID);  // convert char array to int
      Serial.print("new network ID: ");
      Serial.println(nid);
      if(nid > 0 && nid <= 254)
      {
        Serial.println("assigning new network id");
        NETWORKID = nid;
      }
      else
        {Serial.println("network id size errorrrrr");return false;}
    }
    else
      return false;

    // check ENCRYPTKEY
    int _ENCRYPTKEYSIZE = getCharArraySize(_ENCRYPTKEY, sizeof(_ENCRYPTKEY));
    int ENCRYPTKEYSIZE = getCharArraySize(ENCRYPTKEY, sizeof(ENCRYPTKEY));
    if(_ENCRYPTKEYSIZE == ENCRYPTKEYSIZE)
    {
      Serial.println("encrypt key size ok");
      for(int i=0; i<sizeof(_ENCRYPTKEY); i++)
        ENCRYPTKEY[i] = _ENCRYPTKEY[i];
      return true;
    }
    else
      {Serial.println("encrypt key size NOT ok");return false;}
  }

  else if(_dataType[0] == 'u')
  {
    // nothing more to verify, return true
    return true;
  }

  else if(_dataType[0] == 's')
  {
    int sp;
    sscanf(_setpoint, "%d", &sp);  // "%ld" will convert array to long (use "%d" to convert to int)
    setpoint = sp;
    return true;
  }

  else if(_dataType[0] == 'i')
  {
    long i;
    sscanf(_interval, "%ld", &i);  // "%ld" will convert array to long (use "%d" to convert to int)
    transmitInterval = i;
    return true;
  }

  else
    return false;
}

// =================================================================
// count no. of occurences of argument in a char array
// =================================================================
int countOccurence(char* _data, int length, char element)
{
  byte count = 0; // count number of commas in the datapacket
  for (int i=0; i<length; i++)
  {
    if(_data[i] == element)
      count++;
  }
  return count;
}

// =================================================================
// Get actual size of a char array
// =================================================================
int getCharArraySize(char* _data, int length)
{
  byte count = 0;
  for (int i=0; i<length; i++)
  {
    if(_data[i] != '\0')  // count everything except nulls
      count++;
  }
  return count;
}

// =================================================================
