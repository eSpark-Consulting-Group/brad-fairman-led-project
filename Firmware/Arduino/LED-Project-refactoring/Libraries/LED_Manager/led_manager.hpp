#ifndef LED_MANAGER_H
#define LED_MANAGER_H

#include "../../definitions.h"

//----------------------------------------------------------------------
typedef struct
{
    bool        state;                  // led on or off
    uint8_t     step;                   // current step in sequence
    uint64_t    statePeriod;            // time duration for current state
    uint64_t    stateStartTime;         // timer value when this state started
}LED_STATE_t;
//----------------------------------------------------------------------
class OutputLed
{
    public:
        OutputLed(uint8_t redPin, uint8_t whitePin)
        : _redLedPin(redPin), _whiteLedPin(whitePin)
        {   pinMode(_redLedPin, OUTPUT);
            pinMode(_whiteLedPin, OUTPUT);
            digitalWrite(_redLedPin, LOW);
            digitalWrite(_whiteLedPin, LOW);     }
    private:
        uint8_t _redLedPin, _whiteLedPin;
};
//----------------------------------------------------------------------
class StatusLeds
{
    public:
        StatusLeds(uint8_t pin) : _pin(pin)
        {   
            pinMode(_pin, OUTPUT);
            _control = {(STATUS_LEDS_INVERTED) ? HIGH: LOW, 
                        0, 0, 0};
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
    private:
        void updateState();
        uint8_t _pin;
        LED_STATE_t _control;
};
//----------------------------------------------------------------------
class StatusManager : public Observer
{
    public:
        StatusManager(const StatusManager&) = delete;
        StatusManager& operator=(const StatusManager&) = delete;

        static StatusManager* getInstance();
        
        static void statusLedHandlerWrapper();             // callback function handling status LEDs
        
        bool hasStatus(DEVICE_STATE_e status)       {   return (_manager_status & status) == status;    }
        uint8_t getStatus()                  {   return _manager_status;     }
        
        void addLed(StatusLeds* led);
        void BattFlag(BATT_LVL_e batt)              {   _batt_lvl = batt;   }

        void ledOn(uint8_t led);
        void ledOff(uint8_t led);

        void managerTest(DEVICE_STATE_e testStatus, BATT_LVL_e lvl);
        
        void registerEvent(EVENTS_LIST_e eventID, uint8_t val);
    private:
        StatusManager() {   
            for(uint8_t i=0; i<STATUS_LEDS; i++)
            {
                _status_led[i] = nullptr;
            }
            // _batt_lvl = nullptr;
        }
        void setStatus(DEVICE_STATE_e status)  {   _manager_status = status;   }
        void statusLedHandler();             // callback function handling status LEDs

        uint8_t      _bit_flag, _step_counts;
        uint32_t     (*_sequence_ptr[STATUS_LEDS])[LVL_MAX][3];
        uint64_t     _timer_counter = 0;
        uint16_t     _manager_status = 0;
        uint8_t      _registered_leds = 0;
        StatusLeds*  _status_led[STATUS_LEDS];
        BATT_LVL_e   _batt_lvl;
        // DEVICE_STATE_e  _manager_status;

        static StatusManager* _instance;
};


#endif