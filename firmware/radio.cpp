//=========================================================================================================
// radio.cpp - Implements a radio interface to the gateway
//=========================================================================================================
#include "globals.h"
#include <RFM69_ATC.h>

RFM69_ATC radio;

//=========================================================================================================
// maps a structure onto a buffer named "raw"
//=========================================================================================================
#define map_struct(t,m) t& m=*(t*)raw
//=========================================================================================================


//=========================================================================================================
// device_config_t  - This is a config packet we transmit to the gateway when we are in configuration mode
//
// borc_data_t      - This is a telemetry packet we transmit to the gateway every time we wake up from our
//                    sleep cycle (or when the Serial Server "transmit" command is received)
//  
// from_gateway_t   - This contains bit flags that inform the node on what fields need to be updated
//                    1st bit - change setpoint
//                    2nd bit - change manual index
//                    3rd bit - change node ID, network ID, encrypt key
//                    If no bits are set, it's an acknowledgement message
//=========================================================================================================
struct device_config_t
{
    uint8_t     packet_type;
    uint8_t     version;
    uint8_t     device_type;
    uint16_t    firmware_version;
    uint8_t     uid[8];
};

struct borc_telemetry_t
{
    uint8_t     packet_type;
    uint8_t     version;
    uint8_t     setpoint_f;
    uint8_t     manual_index;
    uint8_t     error_byte;
    uint8_t     transaction_id;
    uint16_t    hum;
    uint16_t    temp_f;
    uint16_t    battery;
    uint16_t    servo_pwm;
};

struct from_gateway_t
{
    uint8_t     packet_type;
    uint8_t     tasks_bit_field;
    uint8_t     setpoint_f;
    uint8_t     manual_index;
    uint8_t     network_id;
    uint16_t    node_id;
    uint8_t     encryption_key[16];
};
//=========================================================================================================


//=========================================================================================================
// init() - Initialize the radio
//=========================================================================================================
void CRadio::init()
{
    // Check if radio can be initialized and do so
    if (radio.initialize(RF69_915MHZ, ee.node, ee.network))
    {
        // if initialization was successful, clear error bit
        System.error_byte &= ~RADIO_ERR;

        // Set the encryption key
        radio.encrypt(ee.encrypt);

        // set high transmission power. Must include this only for RFM69HW/HCW!
        radio.setHighPower(); 
    }
    
    // otherwise set error bit
    else
        System.error_byte |= RADIO_ERR;
    
    // Start with transaction ID 0 on bootup
    m_transaction_id = 0;
}
//=========================================================================================================


//=========================================================================================================
// transmit_telemetry() - Transmits a telemetry packet to the gateway and waits for a response back
//=========================================================================================================
void CRadio::transmit_telemetry()
{
    borc_telemetry_t telemetry;
    OneShot timer;
    bool success = false;

    // Fill in the data in the telemetry packet
    telemetry.packet_type      = TELEMETRY_PACKET;
    telemetry.version          = 1;
    telemetry.temp_f           = (int)(System.temp_f * 100);
    telemetry.hum              = (int)(System.hum * 100);
    telemetry.manual_index     = ee.manual_index;
    telemetry.setpoint_f       = ee.setpoint_f;
    telemetry.battery          = Battery.read_voltage();
    telemetry.servo_pwm        = Servo.current_pwm();
    telemetry.error_byte       = System.error_byte;
    telemetry.transaction_id   = m_transaction_id;
    
    // Attempt to send a packet to the gateway 3 times and wait for response
    for (int attempts = 0; attempts < 3; ++attempts)
    {
        // blink LED blue to indicate a packet sent
        Led.set(BLUE, 0);
        delay(10);
        Led.set(OFF, 0);

        radio.send(1, &telemetry, sizeof(telemetry));

        // start a 1 second timer
        timer.start(1000);

        // sit in a loop for 1 second until a response is received
        while (!timer.is_expired())
        {
            // if we receive a response
            if (radio.receiveDone())
            {
                // handle response and break out
                handle_incoming_radio_packet(radio.DATA);
                success = true;
                break;
            }
        }
        if (success) break;
    }

    // increment transaction ID and reset it to 1 if it overflows
    if (++m_transaction_id == 0) m_transaction_id = 1;
}
//=========================================================================================================


//=========================================================================================================
// transmit_config() - Transmits a config packet to the gateway
//=========================================================================================================
void CRadio::transmit_config(int no_of_attempts)
{   
    device_config_t device_config;
    OneShot timer;
    bool success = false;

    // Fill in the node config details in the telemetry packet
    device_config.packet_type       = CONFIG_PACKET;
    device_config.device_type       = 1;
    device_config.version           = 1;
    device_config.firmware_version  = FW_VERSION;
    memcpy(System.uid, device_config.uid, sizeof(System.uid));
    
    // Attempt to send a packet to the gateway 3 times and wait for response
    for (int attempt = 0; attempt < no_of_attempts; ++attempt)
    {
        // blink LED orange to indicate a packet sent
        Led.set(AMBER, 0);
        delay(10);
        Led.set(OFF, 0);

        radio.send(1, &device_config, sizeof(device_config));
        
        // start a 1 second timer
        timer.start(1000);

        // sit in a loop for 1 second until a response is received
        while (!timer.is_expired())
        {
            // if we receive a response
            if (radio.receiveDone())
            {
                // handle response and break out
                handle_incoming_radio_packet(radio.DATA);
                success = true;
                break;
            }
        }
        if (success) break;
    }
}
//=========================================================================================================


//=========================================================================================================
// handle_incoming_radio_packet() - Handles an incoming radio packet
//=========================================================================================================
void CRadio::handle_incoming_radio_packet(const unsigned char* raw)
{    
    // define the radio bits (RB) we want handled in the tasks bit field
    #define RB_SETPOINT        (1 << 0)
    #define RB_MANUAL_INDEX    (1 << 1)
    #define RB_REBOOT          (1 << 2)
    #define RB_NODE_PARAMS     (1 << 3)
    
    // maps the from_gateway_t structure onto "packet"
    map_struct(from_gateway_t, packet);

    // make sure the packet type is a response from the gateway to BORC
    if (packet.packet_type == RESPONSE_PACKET)
    {   
        // if setpoint bit is set
        if (packet.tasks_bit_field & RB_SETPOINT)
        {
            // set the system mode and interface mode to setpoint mode
            ee.run_mode = System.iface_mode = SETPOINT_MODE;

            // set the actual setpoint
            ee.setpoint_f = packet.setpoint_f;
        }
        
        // if manual index bit is set
        if (packet.tasks_bit_field & RB_MANUAL_INDEX)
        {
            // set the system mode and interface mode to manual mode
            ee.run_mode = MANUAL_MODE;
            System.return_to_run_mode();

            // set the manual index
            ee.manual_index = packet.manual_index;
        }

        // if reboot bit is set
        if (packet.tasks_bit_field & RB_REBOOT)
        {   
            // soft reboot system
            System.reboot();
        }
        
        // if node params bit is set
        if (packet.tasks_bit_field & RB_NODE_PARAMS)
        {   
            // set new radio parameters in the EEPROM
            ee.node = packet.node_id;
            ee.network = packet.network_id;
            memcpy(ee.encrypt, packet.encryption_key, sizeof(ee.encrypt));

            // set the node ID
            radio.setAddress(packet.node_id);

            // set the network ID
            radio.setNetwork(packet.network_id);

            // Set the encryption key
            radio.encrypt(packet.encryption_key);
        }
    }

    // blink the LED green to indicate we a received packet
    Led.set(GREEN, 0);
    delay(500);
    Led.set(OFF, 0);
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
