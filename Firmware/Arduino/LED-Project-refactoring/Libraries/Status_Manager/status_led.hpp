#ifndef STATUS_LED
#define STATUS_LED

#include "../../definitions.h"

typedef struct
{
    bool state;                      // led on or off
    uint8_t step;                    // current step in sequence
    uint64_t statePeriod;            // time period for current state
    uint64_t stateStartTime;         // timer value when this sequence started
}LED_STATE_t;

class Leds
{
    public:
        Leds(uint8_t pin) : _pin(pin)
        {   
            pinMode(_pin, OUTPUT);
            _control = {LOW, 0, 0, 0};
            //     .state = LOW,
            //     .step = 0,
            //     .statePeriod = 0,
            //     .stateStartTime = 0,
            // };
            updateState();
        }

        void toggle();
        void on();
        void off();

        uint64_t getStateTime()             {   return _control.statePeriod;      }
        void setStateTime(uint64_t time)    {   _control.statePeriod = time;      }
        uint64_t getStartTime()             {   return _control.stateStartTime;   }
        uint8_t getStep()                   {   return _control.step;      }
        void setStep(uint8_t stp)           {   _control.step = stp;       }
        bool getState()                     {   return STATUS_LEDS_INVERTED ? !_control.state: _control.state;    }
        void updateState();
    private:
        uint8_t _pin;
        LED_STATE_t _control;
};

class StatusManager
{
    public:
        StatusManager(const StatusManager&) = delete;
        StatusManager& operator=(const StatusManager&) = delete;

        static StatusManager* getInstance();
        
        static void statusLedHandler();

        void setStatus(DEVICE_STATE_e status, bool value);
        bool hasStatus(DEVICE_STATE_e status);

        void addLed(Leds* led);
        void addBattFlag(BATT_LVL_e* batt)  {   _batt_lvl_flag = batt;   }
        // Placeholder for usb connection flag set method
        void ledOn(uint8_t led);
        void ledOff(uint8_t led);
    private:
        StatusManager() {   
            for(uint8_t i=0; i<STATUS_LEDS; i++)
            {
                _status_led[i] = nullptr;
            }
            _batt_lvl_flag = nullptr;
        }

        uint64_t    _timer_counter = 0;
        // uint64_t    _timerB1_counter = 0;
        uint32_t    _current_status = 0;
        uint8_t     _registered_leds = 0;
        Leds*       _status_led[STATUS_LEDS];
        BATT_LVL_e*  _batt_lvl_flag;
        // Placeholder for usb connection flag

        static StatusManager* _instance;
};


#endif