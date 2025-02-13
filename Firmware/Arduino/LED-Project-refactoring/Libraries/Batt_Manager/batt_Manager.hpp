#ifndef BATTMANAGER
#define BATTMANAGER

#include "../../definitions.h"
#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"

class BattManager
{
    public:
        BattManager(SFE_MAX1704X* monitor, uint8_t chgPin, uint8_t alrtPin, uint8_t sdaPin, uint8_t sclPin)
        : _batt(monitor)
        {   _chg_pin = chgPin;
            _max17048_alrt_pin = alrtPin;
            _sda_pin = sdaPin;
            _scl_pin = sclPin;
            pinMode(_chg_pin, INPUT);
            pinMode(_max17048_alrt_pin, INPUT);
            attachInterrupt(digitalPinToInterrupt(_chg_pin), battInterruptHandler, FALLING);     // It takes some time for charging IC to assert interrupt pin ~400ms
            attachInterrupt(digitalPinToInterrupt(_max17048_alrt_pin), alrtInterruptHandler, FALLING);     }

        void init();
        void update();
        // bool getUSBState();
        BATT_LVL_e getBattLvl();
        float      getBattVoltage();
        float      getBattSOC();

        void battInterruptHandler();
        void alrtInterruptHandler();

        void subscribe(Observer* observer)
        {
          _observer = observer;
        }
        void unsubscribe(Observer* observer)
        {
          if(_observer == observer)
            _observer = NULL;
        }
    private:
        BATT_LVL_e batt_lvl_percent(float& batt_voltage);
        BATT_LVL_e batt_lvl_percent(float& soc);
        void notify();

        uint8_t _chg_pin, _max17048_alrt_pin, _sda_pin, _scl_pin;
        float _batt_soc, _batt_voltage;
        BATT_LVL_e* _batt_lvl = NULL;
        SFE_MAX1704X* _batt = NULL;
                
        Observer* _observer = NULL;
};

#endif

// ADD BATTERY MANAGEMENT AND USB MANAGEMENT HERE