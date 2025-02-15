#include "Libraries/Button/button.hpp"


void Button::update()
{
    bool reading = captureButtonState();
        
    if (reading != _last_button_state)
    {
        _last_debounce_time = millis();
    }

    if ((millis() - _last_debounce_time) > _debounce_time)
    {
        if (reading != _state)
        {
            _state = reading;

            if (_state)
            {
                _button_press_time = millis();
            }
            else
            {
                unsigned long pressDurationTime = millis() - _button_press_time;
                if (pressDurationTime < LONG_PRESS_TIME)
                {
                    changePressDuration(SHORT_PRESS);
                    // changePressDuration(RELEASED);
                }
                // else {
                //     // _press_duration = RELEASED;
                // }
                changePressDuration(RELEASED);
            }
        }
    
    }
    if (_state)
    {
        unsigned long currentDuration = millis() - _button_press_time;
        if ((currentDuration >= LONG_PRESS_TIME) && (currentDuration < EXTRA_LONG_PRESS_TIME))
        {
            changePressDuration(LONG_PRESS);
        }
        else if (currentDuration >= EXTRA_LONG_PRESS_TIME)
        {
            changePressDuration(EXTRA_LONG_PRESS);
        }
    }

    _last_button_state = reading;
}

BUTTON_STATE_e Button::getPressDuration()
{
    return _press_duration; // Return last detected press duration when released
}

void Button::changePressDuration(BUTTON_STATE_e duration)
{
    if(duration != _press_duration)
    {
        _press_duration = duration;
        _send_notification = true;
        notify();
    }
}
    
bool Button::captureButtonState()
{
    bool state = digitalRead(_pin);
    return (BUTTON_ACTIVE_STATE == HIGH) ? state : !state;
}

void Button::notify()
{
    if(NULL != _observer && _send_notification)
    {
        _observer->registerEvent(BUTTON_EVENT, (uint8_t)getPressDuration());
        _send_notification = false;
    }
}