// Configuration Mode for BORC
// Updated 02/15/2021

// =================================================================
// Read current node ID, network ID and encrypt key from EEPROM
// =================================================================
void getNodeParams()
{
  EEPROM.get(NODEIDADDRESS, NODEID);
  EEPROM.get(NETWORKIDADDRESS, NETWORKID);
  EEPROM.get(ENCRYPTKEYADDRESS, ENCRYPTKEY);
}

// =================================================================
// Write new node ID, network ID and encrypt key to EEPROM
// =================================================================
void updateEEPROM()
{
  EEPROM.put(NODEIDADDRESS, NODEID);
  EEPROM.put(NETWORKIDADDRESS, NETWORKID);
  EEPROM.put(ENCRYPTKEYADDRESS, ENCRYPTKEY);
  EEPROM.put(CONFIGFLAGADDRESS, true);
}

// =================================================================
// Automatically configure node
// =================================================================
void configurationMode()
{
  Serial.println("Configuration mode");
  delay(5);

  // toggle orange LED to indicate configuration mode
  toggleLED(1,1,0);
  
  // clear received datapacket array
  memset(dataReceived, 0, sizeof dataReceived);   

  // create configuration mode test packet
  // strcat(dataReceived, "c,d767c451a32b7e28,2,10,Asdfghjkl#qw!erG");

  // create a configuration mode packet
  createDataPacket('c');

  // save last time for config loop timeout
  last_time = millis();

  // save last time for config packet trasmission
  unsigned long last_time_2 = millis();

  // config loop
  while (true)
  {
    // keep track of current time
    current_time = millis();

    // send data every 2 seconds
    if(current_time - last_time_2 >= 2000) 
    {
      // false argument does not wait for ACK to come back
      sendData(false);

      // reset transmission time
      last_time_2 = millis();
    }

    // check if gateway sent something back
    if(radio.receiveDone())
    {

      // save incoming data in a data packet
      for (byte i = 0; i < radio.DATALEN; i++)
        dataReceived[i] = (char)radio.DATA[i];
      Serial.println(dataReceived);

      // check if packet is valid, i.e., has data and isn't just an empty ACK
      if(sizeof(dataReceived) > 0)
      {
        Serial.println("data received size ok");

        // parse data and verify it
        if(parseData() == true) // good data received
        {
          Serial.println("data parsed ok");

          // update node parameters in EEPROM with new data. This will set config flag to true
          updateEEPROM();

          // set the new node parameters
          setNodeParams();

          // break out of config while loop
          break;
        }

        // if parsing data failed, break out of while loop
        else
        {
          Serial.println("parse fail");
          break;
        }
      }
    }

    // if configuration timeout reached
    if(current_time - last_time >= configurationTimeout)
    {
      Serial.println("timeout");
      
      // set config flag to false
      configFlag == false;
      break;
    }
  }

  // clear array after processing
  memset(dataReceived, 0, sizeof dataReceived);
}
