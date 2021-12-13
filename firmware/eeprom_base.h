#ifndef _EEPROM_BASE_H_
#define _EEPROM_BASE_H_

//=========================================================================================================
// CEEPROM_Base - Base class for a full featured EEPROM manager
// 
// Key features:
//     Portable and fully hardware independent
//     Data corruption detection via 32-bit CRC
//     Optional wear-leveling
//     Optional caching of wear-leveling information for faster read/writes
//     Optional automatic dirty-checking prior to writing to physical EEPROM
//     The ability to "roll-back" a write, as though the write never happened
//     Seamless management of new EEPROM formats
//     Manages storage devices of up to 64K
// 
// -------------------------
// BASIC USAGE OF THIS CLASS
// -------------------------
//     At a bare minimum, your constructor must fill in the first three fields of "m_data", like this:
// 
//            ptr =  Pointer to the beginning of your data structure.  First item in it
//                   must be a const header_t
//
//          length = The length of the entire structure, including the header_t at the top
// 
//          format = The format number of your data structure
// 
// -------------------------------
// DECLARING YOUR DATA STRUCTURE
// -------------------------------
// 
//     Your derived class should declare a structure to be saved in EEPROM, and the first field of that structure 
//     must be a const header_t.   A typical data structure would look something like this:
// 
//         struct data_t
//         {
//             const header_t header = {0};
//             uint32_t some_field;
//             uint16_t some_other_field;
//             uint16_t and_this_field_too;
//             char string_name[20];
//              /* ETC */
//         } data;
// 
// 
// --------------
// DIRTY CHECKING
// --------------
// 
//      "Dirty Checking" is an optional feature that keeps track of whether the data in your data structure has
//      changed since the last time it was read from or written to EEPROM.  With dirty-checking enabled, during a
//      write operation, the data will only be committed to physical EEPROM if the data structure is dirty (i.e., if
//      the data in RAM is different than the data in the physical EEPROM)
// 
//      You can turn on automatic dirty-checking by filling in "m_data.clean_copy" with a pointer to a buffer that
//      is large enough to hold a copy of your entire data structure.
// 
//      If you can't afford the memory for automatic dirty checking, your derived class can set the "m_is_dirty" flag
//      every time your application updates a field in your data structure.  The best way to do this is to declare your
//      data structure as "const" (so the rest of your code can't accidentally write to it), and for each field in 
//      your data structure, write a set_<field_name> routine that calls supplied "set()" template function.
//   
//      For example, if your data structure contains the field "int flag1," your "set_flag1()" routine would
//      look like this:
// 
//              void set_flag1(int value)
//              {
//                  set(data.flag1, value);
//              }
// 
//       That routine can conveniently be expressed in one line:
//  
//             void set_flag1(int value) { set(data.flag1, value); }
//  
//      To turn off dirty-checking entirely, set "m_is_dirty_checking" to false in your constructor.
//
// -------------
// WEAR LEVELING
// -------------
//     If you want automatic wear-leveling, your constructor must fill in at least the first two
//     fields of "m_wl", like this:
//          
//          count = How many storage "slots" to divide the EEPROM into
// 
//           size = How long each slot is, in bytes.  Must be at least as large as the size of your 
//                  data structure
//
//      Optional caching of journaling data:   
//       
//      When wear-leveling is enabled, every time a read() or a write() is performed, the firmware must
//      search for the most recently used slot (in the case of read) or the least recently use slot
//      (in the case of write).  Without journal caching, this involves reading the header from each
//      slot in EEPROM prior to every read or write.   With journal caching enabled, the data neccessary
//      to make least-recently-used/most-recently-used decisions is cached in memory, potentially speeding
//      up read/write operations.
// 
//      To enable the caching of wear-leveling journal data, your constructor should point m_wl.cache to
//      an array of int32_t, with one element for each wear-leveling "slot"
//       
// --------------------------------------
// MANAGING CHANGES TO THE DATA STRUCTURE
// --------------------------------------
//
//      Occasionally it will be neccessary to release new firmware that adds a new field to the data
//      structure.   When that firmware gets released, it will encounter devices in the field that have
//      the old, shorter data structure, and the firmware may want to initialize it to some default value.
// 
//      When read() encounters an EEPROM with an older, shorter data format, it automatically initializes
//      the new fields in the RAM data structure to zero.    You can use the "initialize_new_fields()" routine
//      to initialize new data fields to other values if the need arises
//
//      Here is an example of an "initialize_new_fields()" virtual function to manage the upgrading
//      of older data structures (existing in EEPROMS in the field) to the latest data structure:
// 
/*

    void DerivedClass::initialize_new_fields()
    {
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
    }
*/

//
// -------------------------
// PORTING TO OTHER HARDWARE
// -------------------------
//
//     This class is written in standard C++11 and contains no OS or CPU-specific code.  In order to
//     port it to new hardware (to PC, or to an I2C EEPROM, or to a controller with built in flash, or
//     some other kind of block-oriented storage), it is only neccessary to override two functions:
//
//         // Used to write a block of data to EEPROM 
//         virtual bool write_physical_block(void* src,  uint16_t address, uint16_t length);
//
//         // Used to read a block of data from EEPROM
//         virtual bool read_physical_block(void* dest, uint16_t address, uint16_t length);
//
// ----------
// CHANGE LOG
// ----------
// 
//   Date    Vers  Who  What
// --------------------------------------------------------------------------------------------------------
// 11-Dec-21   1   DWW  Initial release
// 12-Dec-21   2   DWW  Added "set()" template function
// 12-Dec-21   3   DWW  Minor fixes to comments
//=========================================================================================================
#include <stdint.h>

class CEEPROM_Base
{
public:

    // These are the types of errors that can occur
    enum class error_t : char { OK, IO, CRC, BUG };

    // Constructor.
    // *** Derived constructors MUST FILL IN THE m_data DATA DESCRIPTOR *****
    CEEPROM_Base();

    // Call this to disable (or re-enable) dirty checking.  It's enabled by default
    void    enable_dirty_checking(bool flag) { m_is_dirty_checking = flag; }

    // Read the physical EEPROM into the data structure in RAM
    bool    read();

    // Write the data structure in RAM to physical EEPROM.  Setting "force" to true will force the
    // physical write to occur even if dirty-checking is on and the data isn't dirty
    bool    write(bool force = false);

    // Restore the EEPROM and data structure to where it was before the most recent "write()"
    bool    roll_back();

    // Wipes out the data structures in EEPROM
    bool    destroy();

    // Fetch the error code after a failed read, write, roll_back, or destroy operation
    error_t get_error() { return m_error; }

protected:

        
    // Pure virtual function to initialize new fields when the dataformat changes
    virtual void initialize_new_fields() = 0;

    // Pure virtual functions to perform physical I/O to the EEPROM
    virtual bool write_physical_block(void* src,  uint16_t address, uint16_t length) = 0;
    virtual bool read_physical_block (void* dest, uint16_t address, uint16_t length) = 0;

    //-----------------------------------------------------------------------
    // The order of these fields must not be disturbed!  
    //
    // A const header_t MUST BE THE VERY FIRST FIELD IN YOUR DATA STRUCTURE
    //-----------------------------------------------------------------------
    struct header_t
    {
        uint32_t    crc;
        uint32_t    edition;
        uint32_t    magic;
        uint16_t    data_len;
        uint16_t    format;
    };
    //-----------------------------------------------------------------------

    // Data descriptor - describes the user's data structure
    struct { void* ptr; uint16_t length; uint16_t format; void* clean_copy; } m_data;

    // Wear leveling configuration
    struct { uint16_t count; uint16_t size; uint32_t* cache; } m_wl;
    
    // This is the error code set by one of our public API calls
    error_t     m_error;

    // If this is true, we're performing "dirty checking" of the data prior to writing 
    bool        m_is_dirty_checking;

    // If we are "dirty checking", derived classes can set this flag to indicate the data is dirty
    bool        m_is_dirty;

    // Pointer to the wear-leveling cache if it exists
    uint32_t*   m_wl_cache;

    // Returns true if the derived data structure won't fit into a wear-leveling slot
    bool        bug_check();

    // Computes the CRC of the header + data
    uint32_t    compute_crc(uint16_t data_length);

    // Returns the header of the most recent edition of our data structure found in EEPROM
    bool        find_most_recent_edition(header_t*, uint16_t* p_address, int* p_slot = nullptr);

    // Returns the EEPROM address of the least recently used slot
    bool        find_least_recent_address(uint16_t* p_address, int* p_slot);

    // Converts a 0 thru N slot number into an EEPROM address
    uint16_t    slot_to_header_address(int slot);

    // Destroys the header in the specied EEPROM slot
    bool        destroy_slot(int slot);

    // Reports whether the data-structure in RAM is "dirty" (i.e., requires flushing to EEPROM)
    bool        is_dirty();

    // Mark the data in the RAM structure as "clean"
    void        mark_data_as_clean();

    // Builds the wear-leveling cache
    bool        build_wl_cache();

    // Reads a header from EEPROM into RAM
    bool        read_header(header_t* p_result, uint16_t address);

    // A convenience constant
    const int header_size = sizeof(header_t);

    // A convenient method for setting data values when the data structure is declared "const"
    template <class T> void set(const T& dest, T value)
    {
        *(T*)&dest = value;
        m_is_dirty = true;
    }


private:

    // This will be true if we have wear-leveling data cached
    bool        m_is_cached;
};


#endif