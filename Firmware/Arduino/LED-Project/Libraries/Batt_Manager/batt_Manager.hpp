#ifndef BATTMANAGER
#define BATTMANAGER

#include "../../definitions.h"
#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"

class BattManager
{
    public:
        BattManager(SFE_MAX1704X* monitor, uint8_t usbPin, uint8_t alrtPin, uint8_t sdaPin, uint8_t sclPin)
        : _batt(monitor)
        {   _usb_pin = usbPin;
            _max17048_alrt_pin = alrtPin;
            _sda_pin = sdaPin;
            _scl_pin = sclPin;
            attachInterrupt(digitalPinToInterrupt(_usb_pin), usbInterruptHandler, FALLING);
            attachInterrupt(digitalPinToInterrupt(_max17048_alrt_pin), maxInterruptHandler, FALLING);     }
            // pinMode(_usb_pin, INPUT);
            // pinMode(_max17048_alrt_pin, INPUT);      }       // attach interrupt to these pins?

        void setup();
        bool getUSBState();
        BATT_LVL_e getBattLvl();

        static void usbInterruptHandler();
        static void maxInterruptHandler();
    private:
        uint8_t _usb_pin, _max17048_alrt_pin, _sda_pin, _scl_pin;
        BATT_LVL_e* _batt_lvl = NULL;
        SFE_MAX1704X* _batt = NULL;
};

#endif

// ADD BATTERY MANAGEMENT AND USB MANAGEMENT HERE