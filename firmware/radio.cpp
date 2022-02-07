//=========================================================================================================
// radio.cpp - Implements a radio interface to the gateway
//=========================================================================================================
#include "globals.h"
#include <RFM69_ATC.h>

RFM69_ATC radio;

//=========================================================================================================
// telemetry_t - This is a telemetry packet we transmit to the gateway every time we wake up from our
//               sleep cycle (or when the Serial Server "transmit" command is received)
//=========================================================================================================
struct telemetry_t
{
    uint8_t     version;
    uint8_t     setpoint_f;
    uint8_t     manual_index;
    uint8_t     hum;
    uint16_t    temp_f;
    uint16_t    battery;
    uint16_t    servo_pwm;
};
//=========================================================================================================


//=========================================================================================================
// init() - Initialize the radio
//=========================================================================================================
void CRadio::init()
{
    // Initialize the radio
     radio.initialize(RF69_915MHZ, ee.node, ee.network);

    // Set the encryption key
     radio.encrypt(ee.encrypt);

    // must include this only for RFM69HW/HCW!
     radio.setHighPower(); 
}
//=========================================================================================================



//=========================================================================================================
// transmit_telemetry() - Transmits a telemetry packet to the gateway
//=========================================================================================================
void CRadio::transmit_telemetry()
{
    telemetry_t telemetry;

    // Fill in the data in the telemetry packet
    telemetry.version       = 1;
    telemetry.temp_f        = (int)(System.temp_f * 100);
    telemetry.hum           = (int)System.hum;
    telemetry.manual_index  = ee.manual_index;
    telemetry.setpoint_f    = ee.setpoint_f;
    telemetry.battery       = Battery.read_voltage();
    telemetry.servo_pwm     = Servo.current_pwm();

    // Send the packet to the gateway
    radio.send(1, &telemetry, sizeof(telemetry));
}
//=========================================================================================================
