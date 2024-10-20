#ifndef BASICDEF
#define BASICDEF

#include <Arduino.h>
#include <stdint.h>

#define STATUS_LED_75_PIN   PIN_PA1
#define STATUS_LED_50_PIN   PIN_PA2
#define STATUS_LED_25_PIN   PIN_PA4
#define GPOUT_PIN           PIN_PA5
#define BATT_CHG_PIN        PIN_PA6
#define SCL_PIN             PIN_PB0
#define SDA_PIN             PIN_PB1
#define U_INT_PIN           PIN_PB4
#define U_KILL_PIN          PIN_PB5
#define RED_LED_CTRL_PIN    PIN_PC0
#define WHITE_LED_CTRL_PIN  PIN_PC2
#define BUTTON_PIN          PIN_PC1

#define STATUS_LEDS   3

typedef enum
{
    RELEASED,
    SHORT_PRESS,
    LONG_PRESS,
    EXTRA_LONG_PRESS,
}BUTTON_STATE_e;

typedef enum        // white/red LED state
{
    MODE_OFF = 0,
    MODE_1,
    MODE_2,
    MODE_3,
    MODE_4
}MODE_e;

typedef enum {      // batt lvls
    LVL_20,
    LVL_33,
    LVL_66,
    LVL_100,
    LVL_MAX
}BATT_LVL_e;


#endif
