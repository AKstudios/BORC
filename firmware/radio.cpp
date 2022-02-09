//=========================================================================================================
// radio.cpp - Implements a radio interface to the gateway
//=========================================================================================================
#include "globals.h"
#include <RFM69_ATC.h>

RFM69_ATC radio;

//=========================================================================================================
// device_config_t  - This is a config packet we transmit to the gateway when we are in configuration mode
//
// borc_data_t      - This is a telemetry packet we transmit to the gateway every time we wake up from our
//                    sleep cycle (or when the Serial Server "transmit" command is received)
//=========================================================================================================
struct device_config_t
{
    uint8_t     packet_type;
    uint8_t     device_type;
    uint8_t     uid[8];
};

struct borc_data_t
{
    uint8_t     packet_type;
    uint8_t     version;
    uint8_t     setpoint_f;
    uint8_t     manual_index;
    uint16_t    hum;
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

    borc_data_t telemetry_data;

    // Fill in the data in the telemetry packet
    telemetry_data.packet_type   = DATA_PACKET;
    telemetry_data.version       = 1;
    telemetry_data.temp_f        = (int)(System.temp_f * 100);
    telemetry_data.hum           = (int)(System.hum * 100);
    telemetry_data.manual_index  = ee.manual_index;
    telemetry_data.setpoint_f    = ee.setpoint_f;
    telemetry_data.battery       = Battery.read_voltage();
    telemetry_data.servo_pwm     = Servo.current_pwm();
    
    // Send the packet to the gateway
    radio.send(1, &telemetry_data, sizeof(telemetry_data));
}
//=========================================================================================================


//=========================================================================================================
// transmit_config() - Transmits a config packet to the gateway
//=========================================================================================================
void CRadio::transmit_config()
{   
    device_config_t device_config;

    // Fill in the node config details in the telemetry packet
    device_config.packet_type = CONFIG_PACKET;
    device_config.device_type = 1;
    memcpy(device_config.uid, System.uid, sizeof(device_config.uid));
    
    // Send the packet to the gateway
    radio.send(1, &device_config, sizeof(device_config));
}
//=========================================================================================================


//=========================================================================================================
// sleep() - Puts the radio into sleep mode
//=========================================================================================================
void CRadio::sleep()
{ 
    radio.sleep();
}
//=========================================================================================================
