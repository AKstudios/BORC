# -*- coding: utf-8 -*-

# This script sends commands to the BORC and reads back incoming serial data.
# All data is saved in individual csv files for post-processing.

#=========================================================================================================
# Import necessary modules
#=========================================================================================================

import time
import serial
import getopt
import sys

#=========================================================================================================
# Read arguments from cli and return them as a list
#=========================================================================================================
def read_args():

    global COM_port

    # 1st argument is always script name, so remove it from the argument list
    argumentList = sys.argv[1:]
    
    # Options
    options = "hc:n:"
    
    # Long options
    long_options = ["Help", "COM Port = ", "File Name ="]
    
    try:
        # Parsing arguments
        arguments, values = getopt.getopt(argumentList, options, long_options)
        
        # check each argument
        for argument, value in arguments:
            
            # help/description
            if argument in ("-h"):
                print ("Use script like this: logger.py -c <COM Port> -n <filename>")
                sys.exit(1)
            
            # COM port
            elif argument in ("-c"):
                COM_port = value
            
            # file name
            elif argument in ("-n"):
                print (("File name: %s") % value)
    
    except getopt.error as err:
        # output error, and return with an error code
        print (str(err))

#=========================================================================================================
# Main routine
#=========================================================================================================

# Read arguments from cli
read_args()

# try to initialize the serial port
try:
    serialport = serial.Serial(port=COM_port, baudrate=115200, timeout=2) # make sure baud rate is the same

# if it didn't work, throw an exception
except:
    print('Opening serial port failed.')
    sys.exit(1)

# give some delay until serial port is properly opened
time.sleep(3)


# request data from device

# command = 'temp'
# command = bytes(command, 'utf-8')

# command = b'clog start\r\n'
# serialport.write(command)

# command = b'servo 3900\r\n'
# serialport.write(command)

command = b'temp\r\n'
serialport.write(command)

# loop forever
while True:

    # check number of incoming bytes
    incoming_bytes = serialport.in_waiting   # check remaining number of bytes

    # if there is incoming serial data
    if (incoming_bytes):

        # read data line by line
        data = serialport.readline()
        data = data.decode('utf-8')
        print (data)

        # check for temp response
        if (data.startswith('$$>>')):
            temp = data.split()[2]
            print (temp)