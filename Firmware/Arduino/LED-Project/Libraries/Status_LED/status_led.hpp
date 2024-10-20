#ifndef STATUS_LED
#define STATUS_LED

#include "definitions.h"

typedef struct {
    bool state;             // led on or off
    uint8_t step;
    uint64_t time;          // time period for current state
    uint64_t timer;         // timer value when this sequence started
}LED_STATE_t;

class Leds
{
    public:
        Leds(uint8_t pin)
        : _pin(pin)
        {   pinMode(_pin, OUTPUT);  
            _control = {LOW, 0, 0, 0};
            //     .state = LOW,
            //     .step = 0,
            //     .time = 0,
            //     .timer = 0,
            // };
            digitalWrite(_pin, _control.state);
        }

        void toggle();
        void on();
        void off();
        void getTime()               {   return _control.time;      }
        void setTime(uint64_t time)  {   _control.time = time;      }
        void getTimer()              {   return _control.timer;     }
        void getStep()               {   return _control.step;      }
        void setStep(uint8_t stp)    {    _control.step = stp;      }
    private:
        void updateState();
        uint8_t _pin;
        LED_STATE_t _control;
};

class StatusLed
{
    public:
        StatusLed(const StatusLed&) = delete;
        StatusLed& operator=(const StatusLed&) = delete;

        static StatusLed* getInstance();
        
        void statusLedHandler();

        void addLeds(Leds* led1, Leds* led2, Leds* led3);
        void ledOn(uint8_t led);
        void ledOff(uint8_t led);
    private:
        StatusLed() {   }
        
        uint64_t    _timerB1_counter = 0;
        Leds* _status_led[STATUS_LEDS];

        static StatusLed* instance;
};

#endif