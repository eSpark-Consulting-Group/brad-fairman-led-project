#ifndef BATTMANAGER_H
#define BATTMANAGER_H

#include "../../definitions.h"
#include "src/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"

class BattManager
{
    public:
        BattManager(SFE_MAX1704X* monitor, uint8_t chgPin, uint8_t alrtPin, uint8_t sdaPin, uint8_t sclPin)
        : _chg_pin(chgPin), _max17048_alrt_pin(alrtPin), _sda_pin(sdaPin), _scl_pin(sclPin), _batt(monitor)
        {
            pinMode(_chg_pin, INPUT);
            pinMode(_max17048_alrt_pin, INPUT);
            _instance = this;
            // attachInterrupt(digitalPinToInterrupt(_chg_pin), battInterruptHandler, FALLING);     // It takes some time for charging IC to assert interrupt pin ~400ms
            // attachInterrupt(digitalPinToInterrupt(_max17048_alrt_pin), alrtInterruptHandler, FALLING);     }
        }

        void init();
        void update();
        void updateBattStatus();
        // bool getUSBState();
        BATT_LVL_e getBattLvl();
        float      getBattVoltage();
        int        getBattSOC();

        static void battChgInterruptWrapper();
        static void fuelICInterruptWrapper();

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
        BATT_LVL_e batt_lvl_percent(int& soc);
        void notify(EVENTS_LIST_e eventID, uint8_t val);
        
        void chgInterruptHandler();
        void alrtInterruptHandler();

        uint8_t _chg_pin, _max17048_alrt_pin, _sda_pin, _scl_pin;
        float _batt_voltage;
        int   _batt_soc;
        SFE_MAX1704X* _batt = NULL;
        BATT_LVL_e    _batt_level = LVL_MAX;

        bool _is_usb_connected = false;
        bool _is_charging = false;
                
        Observer* _observer = NULL;
        static BattManager* _instance;
};

#endif

// ADD BATTERY MANAGEMENT AND USB MANAGEMENT HERE