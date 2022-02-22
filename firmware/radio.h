//=========================================================================================================
// radio.h - Defines a radio interface to the gateway
//=========================================================================================================
#ifndef _RADIO_H_
#define _RADIO_H_
#include "common.h"

class CRadio
{
public:

    // Call this to initialize the date with network, node, and encryption key fetched from EEPROM
    void    init();

    // Transmits a telemetry packet to the gateway
    void    transmit_telemetry();

    // Transmits a config packet to the gateway
    void    transmit_config(int no_of_attempts);

    // Puts the radio into sleep mode
    void    sleep();

private:
    // transaction ID to keep track of telemetry packets
    // a transaction ID of 0 indicates a bootup
    uint8_t m_transaction_id;

    // Handles an incoming radio packet to the BORC
    void    handle_incoming_radio_packet(const unsigned char* raw);
};

#endif
//=========================================================================================================

