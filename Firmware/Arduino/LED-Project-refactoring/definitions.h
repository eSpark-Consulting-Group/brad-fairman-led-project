#ifndef DEFINITIONS
#define DEFINITIONS

#include <Arduino.h>
#include <stdint.h>

#define STATUS_LED_75_PIN   PIN_PA1     // status led
#define STATUS_LED_50_PIN   PIN_PA2     // status led
#define STATUS_LED_25_PIN   PIN_PA4     // status led
#define GPOUT_PIN           PIN_PA5     // MAX17048 ALRT pin
#define BATT_CHG_PIN        PIN_PA6     // battery charge indicator pin
#define SCL_PIN             PIN_PB0
#define SDA_PIN             PIN_PB1
#define U_INT_PIN           PIN_PB4     // power manager pin, triggers when button pressed for 4secs, initiate device shutdown sequence when triggered
#define U_KILL_PIN          PIN_PB5     // power manager pin, drive low to kill power (turn off LDO)
#define RED_LED_CTRL_PIN    PIN_PC0     // output led
#define WHITE_LED_CTRL_PIN  PIN_PB2     // output led
#define BUTTON_PIN          PIN_PC1

#define BUTTON_ACTIVE_STATE     LOW

#define STATUS_LEDS_INVERTED    false
#define STATUS_LEDS             3
#define DEBOUNCE_TIME           11U
#define BATTERY_LVL_MIN         3.0f
#define BATTERY_LVL_MAX         4.2f
#define LOW_SOC_THRESHOLD       20.0f
#define CRITICAL_SOC_LVL        10.0f

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
    DISCHARGING = 1 << 4,
    POWERED_OFF = 1 << 5,
    UPDATING_EEPROM = 1 << 6,
}DEVICE_STATE_e;

typedef enum
{
    BUTTON_TRIGGERED = 1,
    BATTERY_UPDATE,
    DEVICE_STATUS_UPDATE,
    FUEL_GUAGE_ALRT_ASSERTED,
    BATT_CHARGE_ALRT_ASSERTED,
    SHUTDOWN_SEQUENCE_TRIGGERRED,
    OUTPUT_LED_MODE_CHANGE_TRIGGERED,
    STATUS_LED_HANDLER_TRIGGERED,
}EVENTS_LIST_e;

// Observer (Subscriber) interface 
class Observer {
    public:
        virtual void registerEvent(EVENTS_LIST_e eventID, uint8_t val) = 0;
};

#endif
