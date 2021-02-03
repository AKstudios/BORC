// Debug function for BORC
// Updated 01/20/2021

// =================================================================
// Check error code
// =================================================================
void debug()
{
  checkBattery();
  if(errorCode != 0 && errorCode != 0x80)  // errors detected apart from battery
    blinkLED(1,0,1,50);       // blink purple to indicate error
  else if(errorCode == 0x80)  // battery low
    blinkLED(1,0,0,10);       // blink red to indicate battery low
  else if(errorCode == 0)     // no errors
    blinkLED(0,0,0,0);        // don't blink LED
}

// =================================================================
// Check battery level
// =================================================================
void checkBattery()
{
  float averageADC = readADC(VBUS_SENSE, 5);
  float batt = (averageADC/1023) * 2 * 3.3;  // convert ADC to voltage reading
  if(batt < 3.6)    // battery below 3.6 is low or has problem
    errorCode |= (1<<BATT_ERR);
  else if(batt >= 3.6 && batt <= 4.3)   // battery is nominal
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
