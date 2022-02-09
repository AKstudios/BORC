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
    void    transmit_config();

    // Puts the radio into sleep mode
    void    sleep();
};

#endif
//=========================================================================================================

