# BORC
Battery-Operated Radiator Control

A low-power automatic radiator control that is designed to retrofit on steam radiators that use Honeywell's T104A1040 Zone Valve Operator
uses LowPowerLab's [DualOptiboot](https://github.com/LowPowerLab/DualOptiboot) bootloader for the ATmega1284P.

This repo contains:
- PCB design files and schematic for control board, power board and LED board for the BORC.
- Firmware for a low-power radiator control node
- Logger script to get real-time current data from the BORC

---

### Logger script

The [logger script](https://github.com/AKstudios/BORC/blob/dev/logger/logger.py) enables capturing current usage data over the serial port on the BORC.

#### Prerequisites:
- [Pyserial](https://pypi.org/project/pyserial/)

#### Run:
Run the script using the following command over a command line interface:

```
python logger.py -c COM15 -n filename
```
(change COM port number and filename as needed)

#### Tips:
- The script automatically adds the .csv extension to the filename, so there's no need to include it in the arguments when running the script
- Use -h command to bring a help menu
- If the servo doesn't move in some areas, you can adjust the default min and max limits to the particular servo installed. The default limits should be set to where the servo is guaranteed to move, and beyond these limits, the servo will not respond.
- Default baudrate for the BORC is 115200.
