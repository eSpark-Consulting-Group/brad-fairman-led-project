#ifndef DEFINITIONS
#define DEFINITIONS

#include <Arduino.h>
#include <stdint.h>

#define STATUS_LED_75_PIN   PIN_PA1     // status led
#define STATUS_LED_50_PIN   PIN_PA2     // status led
#define STATUS_LED_25_PIN   PIN_PA4     // status led
#define GPOUT_PIN           PIN_PA5     // MAX17048 ALRT pin
#define BATT_CHG_PIN        PIN_PA6     // usb connected indicator pin
#define SCL_PIN             PIN_PB0
#define SDA_PIN             PIN_PB1
#define U_INT_PIN           PIN_PB4     // power manager pin, triggers when button pressed for 4secs
#define U_KILL_PIN          PIN_PB5     // power manager pin, drive low to kill power
#define RED_LED_CTRL_PIN    PIN_PC0     // output led
#define WHITE_LED_CTRL_PIN  PIN_PB2     // output led
#define BUTTON_PIN          PIN_PC1

#define BUTTON_ACTIVE_STATE LOW

#define STATUS_LEDS_INVERTED    false
#define STATUS_LEDS         3
#define DEBOUNCE_TIME       11U
#define BATTERY_LVL_MIN     3.0f
#define BATTERY_LVL_MAX     4.2f

typedef enum
{
    RELEASED,
    SHORT_PRESS,
    LONG_PRESS,
    EXTRA_LONG_PRESS,
}BUTTON_STATE_e;

typedef enum        // white/red LED modes
{
    MODE_OFF = 0,
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4,
}MODE_e;

typedef enum        // batt lvls
{
    LVL_20,
    LVL_33,
    LVL_66,
    LVL_100,
    LVL_MAX,
}BATT_LVL_e;

typedef enum
{
    FAULT = 1 << 0,
    ACTIVE = 1 << 1,
    STANDBY = 1 << 2,
    CHARGING = 1 << 3,
    POWERED_OFF = 1 << 4,
    UPDATING_EEPROM = 1 << 5,
}DEVICE_STATE_e;

#endif
