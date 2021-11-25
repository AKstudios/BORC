// RFM69 Radio action functions for BORC
// Updated 02/15/2021

// =================================================================
// Function to initialize RFM69 radio
// =================================================================
void initializeRadio()
{
  // initialize radio
  if(!radio.initialize(FREQUENCY,NODEID,NETWORKID))
    errorCode |= (1<<RADIO_ERR);
  else
    errorCode &= ~(1<<RADIO_ERR);
  radio.encrypt(ENCRYPTKEY);
#ifdef IS_RFM69HW_HCW
  radio.setHighPower(); //must include this only for RFM69HW/HCW!
#endif
}

// =================================================================
// Function to create data packet to transmit
// =================================================================
void createDataPacket(char datatype)
{

  Serial.println("creating datapacket");
  delay(5);
  // create configuration datapacket
  if(datatype == 'c')
  {
    // create config data packet
    memset(dataPacket, 0, sizeof dataPacket);   // clear array
    dataPacket[0] = 0;  // first value of dataPacket should be a 0
    strcat(dataPacket, "c,#:");
    strcat(dataPacket, UID);
    strcat(dataPacket, ",$:");
    strcat(dataPacket, NODETYPE);
    strcat(dataPacket, ",@:");
    strcat(dataPacket, FWVERSION);
    delay(1);
    Serial.println(dataPacket);
  }

  // send sensor data
  else if(datatype == 's')
  {
    // create sensor data packet
    memset(dataPacket, 0, sizeof dataPacket);   // clear array
    dataPacket[0] = 0;  // first value of dataPacket should be a 0
    
    char _temp[8];
    char _rh[8];
    char _setpoint[4];
    char _manualOverride[2];
    char _batt[5];
    
    // convert floats and ints to char arrays
    dtostrf(temp, 3, 2, _temp);  // this function converts float into char array. 3 is minimum width, 2 is decimal precision
    dtostrf(rh, 3, 2, _rh);
    dtostrf(setpoint, 1, 0, _setpoint);
    dtostrf(manualOverride, 1, 0, _manualOverride);
    dtostrf(batteryVoltage, 4, 2, _batt);
    
    // merge all values into data packet
    strcat(dataPacket, "!,");   // first exclaimation point (!) indicates a sensor packet
    strcat(dataPacket, "t:");
    strcat(dataPacket, _temp);
    strcat(dataPacket, ",rh:");
    strcat(dataPacket, _rh);
    strcat(dataPacket, ",y:");
    strcat(dataPacket, _setpoint);
    strcat(dataPacket, ",u:");
    strcat(dataPacket, _manualOverride);
    strcat(dataPacket, ",b:");
    strcat(dataPacket, _batt);
    delay(1);
    Serial.println(dataPacket);
  }
}

// =================================================================
// Function to send data packet
// =================================================================
void sendData(boolean checkForACK)
{

//  radio.sendWithRetry(GATEWAYID, dataPacket, strlen(dataPacket));  // send data

  
  Serial.println(servoWDTflag);
  delay(5);
  
  if(checkForACK == true)
  {
    unsigned long last_time = millis(); // save last millis time
    while(1)
    {
      unsigned long current_time = millis();
      if (radio.receiveDone())
      {
        memset(dataReceived, 0, sizeof dataReceived);   // clear array
        for (byte i = 0; i < radio.DATALEN; i++)
          dataReceived[i] = (char)radio.DATA[i];
      }
      
      if(current_time - last_time >= ACKTIMEOUT) // timeout checking for ACK
        break;
    }
  }

  
  Serial.println("checking for ack done");
  delay(5);
}

// =================================================================
// Set new node ID, network ID and encrypt key in RFM69 radio
// =================================================================
void setNodeParams()
{
  radio.setAddress(NODEID);
  radio.setNetwork(NETWORKID);
  radio.encrypt(ENCRYPTKEY);
}

// =================================================================
