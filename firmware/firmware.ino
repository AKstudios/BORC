#include "globals.h"
#include "common.h"
#include <avr/wdt.h>


//=========================================================================================================
// Setup
//=========================================================================================================
void setup()
{    
    Serial.begin(115200);
    Serial.println("begin");

    // blink aqua for 1 second to show device booted
    Led.set(AQUA, 1000, true);

    // << WE WANT THIS TO BE AVAILABLE WHEN THE OTHER .init() ROUTINES ARE CALLED >>>
    EEPROM.read();

    // <<< WE WANT THIS TO BE AVAILABLE WHEN System.init() IS CALLED >>>
    Logger.init();

    // << WE WANT THIS TO BE AVAILABLE WHEN THE OTHER .init() ROUTINES ARE CALLED >>>
    System.init();

    // initialize the power manager
    PowerMgr.init();

    // turn power all for all devices
    PowerMgr.powerOnAll(); 

    // initialize all devices
    Knob.init(CHANNEL_A, CHANNEL_B, CLICK_PIN);
    Display.init(LED_MATRIX_ADDRESS);
    Servo.init();
    Led.init();

    // If we don't have any hardware, simulate normal room temperature
    if (NO_HW) TempHum.simulate_temp_f(75.00);

    // Initilize the Temperature controller
    TempCtrl.init();

    // restore the system orientation from EEPROM
    System.set_orientation(ee.orientation);

    // set system mode to the actual run mode
    System.return_to_run_mode();

  
    // if the servo hasn't been successfully calibrated, do so
    // if (ee.is_servo_calibrated == NOTCAL)  Servo.calibrate_bare();

    // read temp and hum from sensor
    Serial.println(TempHum.read_temp_f());
    Serial.println(TempHum.read_hum());

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