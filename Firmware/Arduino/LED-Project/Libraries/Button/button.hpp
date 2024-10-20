#ifndef BUTTONS
#define BUTTONS

#include "../../definitions.h"

#define SHORT_PRESS_TIME        250
#define LONG_PRESS_TIME         2000
#define EXTRA_LONG_PRESS_TIME   4000

class Button
{
    public:
        // Button(const Button&) = delete;
        // Button& operator=(const Button&) = delete;
        // static Button* getInstance(uint8_t pin, unsigned long time);
        Button(uint8_t pin, unsigned long time)
        : _pin(pin), _debounce_time(time), _last_debounce_time(0), _state(LOW), _last_button_state(LOW), _button_press_time(0), _press_duration(RELEASED)
        {  pinMode(_pin, INPUT_PULLUP);  }

        static void cleanup();

        void update();
        BUTTON_STATE_e getPressDuration();
    private:

        uint8_t _pin;
        unsigned long  _debounce_time, _last_debounce_time;
        bool    _state, _last_button_state;
        uint64_t    _button_press_time;
        BUTTON_STATE_e _press_duration;

        // static Button* instance;
};


#endif