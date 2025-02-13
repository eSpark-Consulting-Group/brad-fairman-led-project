#ifndef BUTTONS
#define BUTTONS

#include "../../definitions.h"

#define SHORT_PRESS_TIME        1000
#define LONG_PRESS_TIME         2000
#define EXTRA_LONG_PRESS_TIME   4000

class Button
{
    public:
        Button(uint8_t pin, unsigned long debounceTime)
        : _pin(pin), _debounce_time(debounceTime), _last_debounce_time(0),
          _button_press_time(0), _press_duration(RELEASED),
          _last_button_state(BUTTON_ACTIVE_STATE),  _state(BUTTON_ACTIVE_STATE)     // set these values as active to avoid false detection at startup since button would be in active state for few ms
        {
          pinMode(_pin, INPUT);
          bool state = (BUTTON_ACTIVE_STATE == LOW) ? HIGH : LOW;
          // _state = state;
          // _last_button_state = state;
        }

        void update();
        BUTTON_STATE_e getPressDuration();

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
        bool captureButtonState();
        void notify();

        uint8_t _pin;
        unsigned long  _debounce_time, _last_debounce_time;
        bool    _state, _last_button_state;
        uint64_t    _button_press_time;
        BUTTON_STATE_e _press_duration;
        
        Observer* _observer = NULL;
};

#endif
