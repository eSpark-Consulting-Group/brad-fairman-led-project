#include "button.hpp"

// Button* Button::instance = nullptr;

// static Button* Button::getInstance(uint8_t pin, unsigned long time)
// {
//     if (!instance) {
//         instance = new button(pin, time);
//     }
//     return instance;
// }

// static void Button::cleanup()
// {
//     delete instance;
//     instance = nullptr;
// }

void Button::update()
{
    bool reading = digitalRead(_pin);
        
    if (reading != _last_button_state)
    {
        _last_debounce_time = millis();
    }

    if ((millis() - _last_debounce_time) > _debounce_time)
    {
        if (reading != _state)
        {
            _state = reading;

            if (_state == BUTTON_ACTIVE_STATE)
            {
                _button_press_time = millis();
            }
            else
            {
                unsigned long pressDurationTime = millis() - _button_press_time;
                if (pressDurationTime < SHORT_PRESS_TIME) {
                    _press_duration = SHORT_PRESS;
                } else {
                    _press_duration = RELEASED;
                }
            }
        }
    }

    if (_state == BUTTON_ACTIVE_STATE) {
        unsigned long currentDuration = millis() - _button_press_time;
        if (currentDuration >= LONG_PRESS_TIME) {
            _press_duration = LONG_PRESS;
        } else if (currentDuration >= EXTRA_LONG_PRESS_TIME) {
            _press_duration = EXTRA_LONG_PRESS;
        }
    }

    _last_button_state = reading;
}

BUTTON_STATE_e Button::getPressDuration() const
{
    // if (_state == HIGH)
    // {
        return _press_duration; // Return last detected press duration when released
    // }
    // return RELEASED; // If still pressed, return RELEASED state
}
