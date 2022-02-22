#include "globals.h"
#include "common.h"
#include <avr/wdt.h>
#include "i2c.h"


//=========================================================================================================
// Setup
//=========================================================================================================
void setup()
{    
    Serial.begin(115200);
    Serial.println("begin");

    // Use a system-wide I2C bus speed of 400 Khz
    twi_setFrequency(400000);

    // blink aqua for 1 second to show device booted
    Led.init();
    Led.set(AQUA, 1000, true);

    // << WE WANT THIS TO BE AVAILABLE WHEN THE OTHER .init() ROUTINES ARE CALLED >>>
    EEPROM.read();

    // <<< WE WANT THIS TO BE AVAILABLE WHEN System.init() IS CALLED >>>
    Logger.init();

    // << WE WANT THIS TO BE AVAILABLE WHEN THE OTHER .init() ROUTINES ARE CALLED >>>
    System.init();
    
    // Fetch the current temperature
    SHT31.read_f(&System.temp_f);

    // initialize the power manager
    PowerMgr.init();

    // turn power all for all devices
    PowerMgr.powerOnAll(); 

    // initialize all devices
    INA219.init(CURRENT_SENSE_ADDRESS);
    Knob.init(CHANNEL_A, CHANNEL_B, CLICK_PIN);
    Display.init(LED_MATRIX_ADDRESS);
    Servo.init();
    Radio.init();

    // Initilize the Temperature controller
    TempCtrl.init();

    // restore the system orientation from EEPROM
    System.set_orientation(ee.orientation);

    // set system mode to the actual run mode
    System.return_to_run_mode();
  
    // if the servo hasn't been successfully calibrated, do so
    // if (ee.is_servo_calibrated == NOTCAL)  Servo.calibrate_bare();

    // Transmit a packet of telemetry on bootup
    Radio.transmit_telemetry();
    Radio.transmit_config(3);

    //-------------------------------------------------------
    //              FROM HERE DOWN IS DEBUG CODE
    //-------------------------------------------------------
    // read temp and hum from sensor
    float temp_f, rh;
    if (SHT31.read_f(&temp_f, &rh))
    {
      aprintf("Temperature: %1.2f, RH: %1.2f\n", temp_f, rh);
    }

    // Servo.move_to_pwm(92, 4000, true);
    // Servo.move_to_pwm(542, 4000, true);  

}
//=========================================================================================================

//=========================================================================================================
// BIG LOOP
//=========================================================================================================
void loop()
{
    wdt_reset(); //pat the dog...

    USB.execute();

    SerialServer.execute();

    SleepMgr.execute();

    Led.execute();
  
    switch (System.iface_mode)
    {
      case MANUAL_MODE   : ManualModeMgr.execute();    break;
      case MENU_MODE     : MenuMgr.execute();          break;
      case SETPOINT_MODE : SetpointModeMgr.execute();  break;
    }
}
//=========================================================================================================
