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
void updateNodeParamsInEEPROM()
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
  memset(dataReceived, 0, sizeof dataReceived);   // clear array
  
//  strcat(dataReceived, "c,d767c451a32b7e28,2,10,Asdfghjkl#qw!erG"); // test
   
  Serial.println("Configuration mode");
  delay(5);
  toggleLED(1,1,0); // toggle orange LED to indicate configuration mode

  createDataPacket('c');  // create a configuration packet
  
  last_time = millis();
  unsigned long last_time_2 = millis();
  while (true)
  {
    current_time = millis();  // keep track of current time

    if(current_time - last_time_2 >= 2000) // send data every 2 seconds
    {
      sendData(false);  // false argument does not wait for ACK to come back
      last_time_2 = millis();
    }

    // check if gateway sent something back
    if(radio.receiveDone())
    {
      for (byte i = 0; i < radio.DATALEN; i++)
        dataReceived[i] = (char)radio.DATA[i];

      Serial.println(dataReceived);
      if(sizeof(dataReceived) > 0)  // valid packet, not just an empty ACK
      {
        Serial.println("data received size ok");          
        if(parseData() == true) // good data received
        {
          Serial.println("data parsed ok");
          updateNodeParamsInEEPROM();
          setNodeParams();
          break;
        }
        else{
          Serial.println("parse fail");
          break;}
      }
    }

    // timeout
    if(current_time - last_time >= configurationTimeout)
    {
      Serial.println("timeout");
      configFlag == false;
      break;
    }
  }
  memset(dataReceived, 0, sizeof dataReceived);   // clear array after processing
}
