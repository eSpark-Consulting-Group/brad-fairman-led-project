#ifndef DEVICE
#define DEVICE

#include "definitions.h"
#include "timer_interface.h"
#include "status_led.hpp"
#include "output_led.hpp"
#include <EEPROM.h>

#define EEPROM_ENDURANCE    100000
#define DEVICE_ID           0x1616

#define EEPROM_ID_HIGH_BYTE       (EEPROM.begin())
#define EEPROM_ID_LOW_BYTE        (EEPROM.begin()+1)
#define EEPROM_COUNT_ADDRESS      (EEPROM.begin()+2)
#define EEPROM_MODE_REF_ADDRESS   (EEPROM.begin()+3)
#define EEPROM_MODE_ADDRESS       (EEPROM.begin()+4)

class Device
{
    public:
        Device(OutputLed* outputLedController, StatusLed* statusLedController, Leds* led1, Leds* led2, Leds* led3)
        : _status_led_controller(statusLedController), _device_id(DEVICE_ID), _current_mode(MODE_1),
          _output_led_controller(outputLedController)
        {   _status_led_controller->addLeds(led1, led2, led3);   }

        void setup();

        void eepromModeWrite(MODE_e mode);
        
        MODE_e _current_mode;
    private:
        void eepromSetup();
        void eepromInit();

        uint32_t    _eeprom_write_count;
        uint8_t     _eeprom_mode_addr;
        uint16_t    _device_id;

        TMR_INTERFACE* _pwm_timer, _status_led_timer;
        StatusLed* _status_led_controller;
        OutputLed* _output_led_controller;
};

//----------------------------------------------------------------------

void disableUnusedPeripherals();


#endif