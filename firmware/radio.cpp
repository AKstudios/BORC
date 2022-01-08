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
    uint8_t     temp_f;
    uint8_t     setpoint_f;
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
}
//=========================================================================================================



//=========================================================================================================
// transmit_telemetry() - Transmits a telemetry packet to the gateway
//=========================================================================================================
void CRadio::transmit_telemetry()
{
    telemetry_t telemetry;

    // Fill in the data in the telemetry packet
    telemetry.version    = 1;
    telemetry.temp_f     = (int)(System.temp_f + .5);
    telemetry.setpoint_f = ee.setpoint_f;
    telemetry.battery    = analogRead(VBUS_SENSE);
    telemetry.servo_pwm  = Servo.current_pwm();

    // Send the packet to the gateway
    radio.send(1, &telemetry, sizeof(telemetry));
}
//=========================================================================================================
