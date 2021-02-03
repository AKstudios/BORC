// Configuration file for BORC
// Updated 01/20/2021

// =================================================================
// Global variables
// =================================================================
#define SERIAL_BAUD 115200
byte setpointMaxValue = 90;
byte setpointMinValue = 55;
int setpoint = 72;
int knobDirection = 0;
byte errorCode = 0;
float busVoltage, motorCurrent;
float temp, temp_f, rh;
unsigned long last_time, current_time;
boolean current_A_state, last_A_state;
byte transmitIntervalCounter = 0;
byte actionsIntervalCounter = 0;
int displayTimeout = 5000;
int knobCounter = 0;
String displayText;

// Node configuration ----------------------------------------------
#define NODEID                    999 // 999 = node not configured; 1000 = node currently being configured (limit: 1023)
#define NETWORKID                 10  // set by gateway (up to 255)
#define GATEWAYID                 1   // for main gateway
#define FREQUENCY                 RF69_915MHZ // 915 Mhz for NA region
#define ENCRYPTKEY                "sampleEncryptKey" // 16 bytes, set by gateway
#define IS_RFM69HW_HCW            //uncomment only for RFM69HW/HCW! Leave out if you have RFM69W/CW!

// Pin mapping -----------------------------------------------------
#define KNOB_CLICK                0
#define GREEN                     3
#define CHANNEL_B                 10
#define CHANNEL_A                 11
#define MOTOR_IN1                 12
#define MOTOR_IN2                 13
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
#define MOTOR_POWER_ERR           4   // 12V power not present
#define FLASH_ERR                 5   // SPI flash error
#define RADIO_ERR                 6   // Radio error
#define MOTOR_ERR                 7   // motor error (0 current or very high current)

// All flags -------------------------------------------------------
boolean displayMax = false;
boolean displayMin = false;
boolean configFlag = false;
boolean WDTflag = false;
boolean knobFlag = false;
boolean knobClickFlag = false;
boolean advancedOptions = false;
boolean manualMode = false;
boolean calibrationMode = false;

// =================================================================
