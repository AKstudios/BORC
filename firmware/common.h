// Misc. global variables ------------------------------------------
#define SERIAL_BAUD               115200
#define FWVERSION                 "1007"

// I2C Addresses ---------------------------------------------------
#define CURRENT_SENSE_ADDRESS     0x41
#define SERVO_DRIVER_ADDRESS      0x40
#define TEMP_SENSE_ADDRESS        0x45
#define LED_MATRIX_ADDRESS        0x74

// Pin mapping -----------------------------------------------------
#define CLICK_PIN                 0
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


