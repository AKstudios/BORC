#include <avr/eeprom.h>
#include <string.h>
#include "eeprom_manager.h"


//=========================================================================================================
// initialize_new_fields() - This is called when the EEPROM has been read in and has a format
//                           that is older than the current format supported by this firmware
//=========================================================================================================
void CEEPROM::initialize_new_fields()
{
/*
      // This is an example for how to initialize "new_field_1" and "new_field_2" that were
      // added to our data structure in DATA_FORMAT #2
         
      if (data.header.format < 2)
      {
          new_field_1 = some_default_value;
          new_field_2 = some_default_value;
          (etc)
      }
 
      // And we also initialize "another_new_field" that was added to our data structure 
      // in DATA_FORMAT #3

      if (data.header.format < 3)
      {
          another_new_field = some_default_value;
          (etc)
      }
*/
}
//=========================================================================================================



//=========================================================================================================
// Constructor() - Calls the base class, fills in the data descriptor, and configures wear-leveling
//=========================================================================================================
CEEPROM::CEEPROM() : CEEPROM_Base()
{
    // Fill in the data descriptor, including automatic dirty-checking
    m_data = { &data, sizeof(data), DATA_FORMAT, &clean };

    // Fill in the wear-leveling configuration, including journal caching
    m_wl = { WEAR_LEVELING_SLOTS, WEAR_LEVELING_SIZE, m_cache_buffer };
}
//=========================================================================================================





//=========================================================================================================
// write_physical_block() - Writes a block of data to the specified EEPROM address
//=========================================================================================================
bool CEEPROM::write_physical_block(void* src, uint16_t address, uint16_t length)
{
    // Use the AVR API to write the block from RAM into EEPROM
    eeprom_update_block(src, (void*)(address), length);

    // The AVR routines don't return a status, so we have to just assume they worked
    return true;
};
//=========================================================================================================



//=========================================================================================================
// read_physical_block() - Reads a block of data from the specified EEPROM address
//=========================================================================================================
bool CEEPROM::read_physical_block(void* dest, uint16_t address, uint16_t length)
{
    // Use the AVR API to read the block from EEPROM into RAM
    eeprom_read_block(dest, (void*)(address), length);

    // The AVR routines don't return a status, so we have to just assume they worked
    return true;
}
//=========================================================================================================

