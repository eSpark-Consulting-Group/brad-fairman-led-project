#ifndef BUTTONS_H
#define BUTTONS_H

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
          _state(BUTTON_ACTIVE_STATE), _last_button_state(BUTTON_ACTIVE_STATE)     // set these values as active to avoid false detection at startup since button would be in active state for few ms
        {
          pinMode(_pin, INPUT);
          // _state = digitalRead(_pin);
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
        void changePressDuration(BUTTON_STATE_e duration);
        bool captureButtonState();
        void notify();

        uint8_t _pin;
        unsigned long  _debounce_time, _last_debounce_time;
        uint64_t    _button_press_time;
        BUTTON_STATE_e _press_duration;
        bool    _state, _last_button_state;
        bool    _send_notification;
        
        Observer* _observer = NULL;
};

#endif
