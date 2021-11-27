// RGB LED control for BORC
// Updated 02/12/2021

// =================================================================
// Blink LED
// =================================================================
void blinkLED(byte R, byte G, byte B, int DELAY_MS)
{
  // set pin modes to output
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // write requested LED value
  digitalWrite(RED,R);
  digitalWrite(GREEN,G);
  digitalWrite(BLUE,B);

  // delay requested time
  delay(DELAY_MS);

  // turn LEDs off
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);

  // set pin modes back to input
  pinMode(RED, INPUT);
  pinMode(GREEN, INPUT);
  pinMode(BLUE, INPUT);
}

// =================================================================
// Fade LED
// =================================================================
void fadeLED(int pin)
{
  int brightness = 0;
  int fadeAmount = 5;
  for(int i=0; i<510; i=i+5)  // 255 is max analog value, 255 * 2 = 510
  {
    analogWrite(pin, brightness);  // pin 9 is LED
  
    // change the brightness for next time through the loop:
    brightness = brightness + fadeAmount;  // increment brightness level by 5 each time (0 is lowest, 255 is highest)
  
    // reverse the direction of the fading at the ends of the fade:
    if (brightness <= 0 || brightness >= 255)
    {
      fadeAmount = -fadeAmount;
    }
    // wait for 20-30 milliseconds to see the dimming effect
    delay(10);
  }
  digitalWrite(pin, LOW); // switch LED off at the end of fade
}

// =================================================================
// Toggle LEDs manually
// =================================================================
void toggleLED(byte R, byte G, byte B)
{
  if (R==0 && G==0 && B==0) // set pin modes and turn off LEDs
  {
    // turn LEDs off
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);

    // set pin modes back to input
    pinMode(RED, INPUT);
    pinMode(GREEN, INPUT);
    pinMode(BLUE, INPUT);
  }

  else  // set pin modes and toggle LEDs
  {
    // set pin modes to output
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
  
    // write requested LED value
    digitalWrite(RED,R);
    digitalWrite(GREEN,G);
    digitalWrite(BLUE,B);
  }
}

// =================================================================
