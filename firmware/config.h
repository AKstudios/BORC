// Configuration file for BORC
// Updated 09/15/2021

// =================================================================
// Global variables
// =================================================================

// Firmware version number -----------------------------------------
#define FWVERSION "1007"

// Misc. global variables ------------------------------------------
#define SERIAL_BAUD 115200
float batteryVoltage;
byte transmitIntervalCounter = 0;
byte actionsIntervalCounter = 0;
unsigned long last_time, current_time;
byte errorCode = 0;

// I2C Addresses ---------------------------------------------------
#define CURRENT_SENSE_ADDRESS 0x41
#define SERVO_DRIVER_ADDRESS  0x40
#define TEMP_SENSE_ADDRESS    0x45
#define LED_MATRIX_ADDRESS    0x74

// Servo configuration ---------------------------------------------
#define SERVO_TYPE  0           // 0 = 270 servo, 1 = 360 servo
#define SERVO_FREQ 50           // analog servos are usually 50 Hz
int servoIncrements = 10;       // for 270 servo
int servoPosition = 150;        // for 270 servo
int servoMinPosition = 90;      // for 270 servo
int servoMaxPosition = 550;     // for 270 servo
int servoCWpulse = 150;         // for 360 servo
int servoCCWpulse = 500;        // for 360 servo
int servoTimeIncrements = 100;  // in ms, for 360 servo

// LED matrix configuration ----------------------------------------
char displayMode, _displayMode;
int displayTimeout = 4000;
String displayText;
int menuOption = 0;

// knob configuration ----------------------------------------------
//boolean current_A_state, last_A_state;
boolean aState, bState, aFlag, bFlag;
int knobTimeout = 4000;
int configurationTimeout = 10000;
int knobDirection = 0;
byte clickDebounceInterval = 400;
byte rotateDebounceInterval = 50;

// Sensor values ---------------------------------------------------
float temp, temp_f, rh;
float busVoltage, current;
byte setpointMaxValue = 90;
byte setpointMinValue = 55;
byte autpoSetpointMaxValue = 90;
byte autpoSetpointMinValue = 55;
byte manualSetpointMaxValue = 6;
byte manualSetpointMinValue = 0;
int setpoint = 72;
byte manualOverride = 0;

// Node & radio configuration --------------------------------------
char _UID[17], _NODEID[5], _NETWORKID[4], _ENCRYPTKEY[17], _setpoint[3], _interval[6];
char UID[17];                     // unique 8-byte (64-bit) ID for node
String _MACID;
char dataPacket[100];                     // store data packet to send
char dataReceived[100];                   // store received data packets
uint16_t NODEID = 999;                    // 999 = node not configured; 1000 = node currently being configured (limit: 1023)
uint8_t NETWORKID = 99;                   // 99 = configuration network, 10 = sensor network (up to 255)
char ENCRYPTKEY[17] = "sampleEncryptKey"; // 16 bytes + null character at the end
long transmitInterval = 56;               // transmit interval for radio messages in seconds

#define CONFIGFLAGADDRESS         10      // config flag's address in EEPROM
#define NODEIDADDRESS             100     // Node ID's address in EEPROM
#define NETWORKIDADDRESS          200     // Encrypt Key's address in EEPROM
#define ENCRYPTKEYADDRESS         300     // Encrypt Key's address in EEPROM
#define NODETYPE                  "ARC"   // 3 letter node type identifier
#define GATEWAYID                 1       // for main gateway
#define FREQUENCY                 RF69_915MHZ // 915 Mhz for NA region
#define ACKTIMEOUT                100
#define IS_RFM69HW_HCW            //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!
//#define NODEID                    999     // 999 = node not configured; 1000 = node currently being configured (limit: 1023)
//#define NETWORKID                 10      // set by gateway (up to 255)
//#define ENCRYPTKEY                "sampleEncryptKey" // 16 bytes, set by gateway

// Pin mapping -----------------------------------------------------
#define KNOB_CLICK                0
#define GREEN                     3
#define CHANNEL_B                 10
#define CHANNEL_A                 11
#define RED                       14
#define BLUE                      15
#define DRIVER_POWER_PIN          18
#define LED_SCREEN_POWER_PIN      19
#define TEMP_SENSOR_POWER_PIN     20
#define CURRENT_SENSE_POWER_PIN   21
#define SERVO_POWER_PIN           22
#define FLASH_SS                  23
#define VBUS_SENSE                A0
#define TEMP_SENSE                A3
#define VBAT_SENSE                A7

// Define error code bits ------------------------------------------
#define BATT_ERR                  0   // battery low/bad voltage
#define TEMP_SENSE_ERR            1   // temp sensor not working
#define LED_DRV_ERR               2   // LED driver not working
#define CURRENT_SENSE_ERR         3   // current sensor not working
#define SERVO_POWER_ERR           4   // servo voltage not correct
#define FLASH_ERR                 5   // SPI flash error
#define RADIO_ERR                 6   // Radio error
#define SERVO_ERR                 7   // servo error (0 current or very high current)

// All flags -------------------------------------------------------
boolean displayMax = false;
boolean displayMin = false;
boolean configFlag = false;
boolean WDTflag = false;
boolean servoWDTflag = false;
boolean knobRotateFlag = false;
boolean knobClickFlag = false;
boolean advancedOptions = false;
boolean manualMode = false;
boolean demoMode = false;
boolean displayFlag = false;
boolean actionsFlag = false;
boolean transmitFlag = false;
boolean quickWDT = false;

// =================================================================
