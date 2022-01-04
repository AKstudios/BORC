# -*- coding: utf-8 -*-

# This script sends commands to the BORC and reads back incoming serial data.
# All data is saved in individual csv files for post-processing.

#=========================================================================================================
# Import necessary modules
#=========================================================================================================
from datetime import datetime
import time
import serial
import getopt
import sys
import csv
#=========================================================================================================


#=========================================================================================================
# Global variables
#=========================================================================================================
DEFAULT_MIN_LIMIT = 3595
DEFAULT_MAX_LIMIT = 3930
servo_range = DEFAULT_MAX_LIMIT - DEFAULT_MIN_LIMIT
notches = 6
buffer = []
file_extention = ".csv"
header = ['Time', 'CURRENT', 'mA']
#=========================================================================================================


#=========================================================================================================
# Read arguments from cli and return them as a list
#=========================================================================================================
def read_args():

    global COM_port
    global filename

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
                filename = timestamp + value   
    
    except getopt.error as err:
        # output error, and return with an error code
        print (str(err))
#=========================================================================================================


#=========================================================================================================
# move servo 
#=========================================================================================================
def move_servo(direction):

    # move servo from low to high
    if (direction == 0):
        send_serial_data('servo %d' % DEFAULT_MAX_LIMIT)
        save_data('0-6')

    # move servo from high to low
    elif (direction == 1):
        send_serial_data('servo %d' % DEFAULT_MIN_LIMIT)
        save_data('6-0')     

    # make servo move from one end to the other in increments
    else:
        
        for index in range(notches):

            # compute the next pwm value
            if (direction == 2):
                pwm_value = index * servo_range / notches + DEFAULT_MIN_LIMIT
                fn = str(index) + '-' + str(index+1)

            elif (direction == 3):
                pwm_value = DEFAULT_MAX_LIMIT - index * servo_range / notches
                fn = str(notches-index) + '-' + str(notches-index-1)

            # send command over serial
            send_serial_data('servo %d' % int(pwm_value))

            # get and save serial data
            save_data(fn)
            
            # wait a sec before moving to next position
            time.sleep(1)
#=========================================================================================================


#=========================================================================================================
# send data over serial
#=========================================================================================================
def send_serial_data(command):

    global buffer

    # add trailing newline and return to command
    command = str(command) + '\r\n'
    
    # send command over serial encoded in bytes format
    serialport.write(command.encode('utf-8'))

    # clear buffer list
    buffer = []

    # loop forever
    while True:

        # read data buffer line by line into a list
        data = serialport.readline().decode('utf-8').strip()

        # if we see a data line,  
        if (data.startswith('$:')):

            # remove spaces, remove leading marker, create a list separated by commas
            data = data.replace(' ', '').replace('$:,','').split(',')

            # and append to the main buffer list
            buffer.append(data)
            # print (data)

        # check for end marker
        elif (data.startswith('$$>>')):
            # end of data stream
            if (data == '$$>> OK' or data == '$$>> FAIL SYNTAX'):
                break
            else:
                datapoint = data.split()[2]
                print ('Room Temp: ' + str(datapoint) + ' ' + u'\N{DEGREE SIGN}' + 'F')
                break

#=========================================================================================================


#=========================================================================================================
# save data to file
#=========================================================================================================
def save_data(fn):

    global buffer
    global filename

    file_name = filename + '_' + fn + file_extention
    
    print (("Writing data to file: %s") % file_name)

    # open file in write mode and write 
    with open(file_name, 'w', newline='') as csvfile:
        csvwriter = csv.writer(csvfile)
        csvwriter.writerow(header)
        csvwriter.writerows(buffer)

    # close file
    csvfile.close()
#=========================================================================================================


#=========================================================================================================
# Main routine
#=========================================================================================================

print ('----- Servo current draw logger -----')

# get current datetime
timestamp = datetime.now().strftime('%Y-%m-%d_%H%M%S_')

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
time.sleep(5)

# get temperature
send_serial_data('temp')

# start current logger
# command = b'clog start\r\n'
send_serial_data('clog start')

# move servo and log data
for i in range(4):
    move_servo(i)

# stop current logger
send_serial_data('clog stop')

print ('----------- DONE -----------')

#=========================================================================================================
