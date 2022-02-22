#include "globals.h"


//=========================================================================================================
// init() - Called once at boot, immediately after the EEPROM has been read
//=========================================================================================================
void CSystem::init()
{
    // initialize error byte with no errors
    System.error_byte = 0x00; 

    // If the EEPROM is blank, use a default setpoint
    if (ee.setpoint_f == 0) ee.setpoint_f = DEFAULT_SETPOINT;

    // If the notch count in EEPROM is blank, 4 is a nice default
    if (ee.notches == 0) ee.notches = 4;

    // Don't allow the user to set an invalid number of notches
    if (ee.notches > CNotchController::MAX_NOTCHES) ee.notches = CNotchController::MAX_NOTCHES;

    // If the time-constant multiplier in EEPROM is blank, set a nice default
    if (ee.tcm == 0) ee.tcm = 2;

    // If the deadband size in EEPROM is blank, set a reasonable default
    if (ee.deadband == 0) ee.deadband = 1;

    // Initialize the flash and get the System Unique ID
    if (Flash.initialize())
    {
      memcpy(System.uid, Flash.readUniqueId(), sizeof(System.uid));
      
      // flash initialization was successful, clear error bit
      System.error_byte &= ~FLASH_ERR;
    }

    // if flash initialization was not successful..
    else
    { 
      // save Unique ID as 00000000
      memcpy(System.uid, '00000000', sizeof(System.uid));

      // and set error bit
      System.error_byte |= FLASH_ERR;
    }
}
//=========================================================================================================


//=========================================================================================================
// reboot() - soft reboots the system
//=========================================================================================================
void CSystem::reboot()
{
    // reboot system
    asm volatile ("jmp 0");
}
//=========================================================================================================


//=========================================================================================================
// return_to_run_mode()
//=========================================================================================================
void CSystem::return_to_run_mode()
{
    if (ee.run_mode == MANUAL_MODE)
        ManualModeMgr.start();
    else
        SetpointModeMgr.start();
}
//=========================================================================================================


//=========================================================================================================
// rotate()
//=========================================================================================================
void CSystem::rotate()
{
  set_orientation(!ee.orientation);
}
//=========================================================================================================


//=========================================================================================================
// set_orientation()
//=========================================================================================================
void CSystem::set_orientation(bool orientation)
{
  ee.orientation = orientation;

  Knob.set_orientation(ee.orientation);

  Display.set_orientation(ee.orientation);
}
//=========================================================================================================
