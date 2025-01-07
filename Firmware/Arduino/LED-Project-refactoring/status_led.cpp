#include "Libraries/Status_Manager/status_led.hpp"

#define LED_BLINK_STEPS_DISCHG 3
uint32_t led1_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{1000, 1000, 1000}, {3000,0,0}, {3000,0,0}, {3000,0,0}};
uint32_t led2_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {3000,0,0}, {3000,0,0}};
uint32_t led3_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {0,0,0},    {3000,0,0}};

#define LED_BLINK_STEPS_CHG 2
uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{1000, 1000}, {1000,1},      {1000,1},       {1000,1}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {1000,1000},   {1000,1},       {1000,1}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {0,0},         {1000,1000},    {1000,1}};

#define LED_BLINK_EEPROM_UPDATE 2
uint32_t led_blink_eeprom[LED_BLINK_EEPROM_UPDATE]       = {50, 50};

//----------------------------------------------------------------------

void Leds::toggle()
{
    _control.state = (_control.state == LOW) ? HIGH : LOW;
    updateState();
}

void Leds::on()
{
    if(STATUS_LEDS_INVERTED)
        _control.state = LOW;
    else
        _control.state = HIGH;

    updateState();
}

void Leds::off()
{
    if(STATUS_LEDS_INVERTED)
        _control.state = HIGH;
    else
        _control.state = LOW;

    updateState();
}

void Leds::updateState()
{
    digitalWrite(_pin, _control.state);
    _control.step++;
    _control.timer = millis();
}

//----------------------------------------------------------------------

StatusManager* StatusLed::_instance = nullptr;

static StatusManager* StatusLed::getInstance()
{
    if (!_instance) {
        _instance = new StatusLed();
    }
    return _instance;
}

void StatusManager::addLed(Leds* led)
{
    if (_registered_leds < STATUS_LEDS)
        _status_led[_registered_leds++] = led;
}

void StatusManager::ledOn(uint8_t led)
{
    if((led > STATUS_LEDS) || (led == 0))
        return;
    
    _status_led[led-1]->on();
}

void StatusManager::ledOff(uint8_t led)
{
    if((led > STATUS_LEDS) || (led == 0))
        return;
    
    _status_led[led-1].off();
}

void StatusManager::setStatus(DEVICE_STATE_e status, bool value)
{
    if (value)
    {
        if (_current_status & status)
        {
            return;
        }
        _current_status = status;
        // _current_status |= status;
    }
    // else
    // {
    //     if (!(_current_status & status))
    //     {
    //         return;
    //     }
    //     _current_status &= ~status;
    // }
}

bool StatusManager::hasStatus(DEVICE_STATE_e status)
{
    return (_current_status & status) == status;
}

static void StatusManager::statusLedHandler()
{
    unsigned long time = millis();
    unsigned long diff = time - _timer_counter;

    if (diff < 5)
    {
        return;
    }

    _timer_counter = time;

    // _timerB1_counter++;
    static int step_count = 0;

    // if(!is_usb_connected)
    // {
    //     for( uint8_t i=0; i<STATUS_LEDS; i++ )
    //     {
    //         if(((_timerB1_counter-_status_led[i]->getTimer()) >= _status_led[i]->getTime()) && !(step_count & (1 << i) ))
    //         {
    //             if (_status_led[i]->getStep() == (LED_BLINK_STEPS_DISCHG-1) || (_status_led[i]->getTime() == 0))
    //             {
    //                 step_count |= (1 << i);
    //             }
    //             else
    //             {
    //                 led_control_dischg(batt_lvl, ++_status_led[i]->getStep(), i);
    //                 status_LED_toggler(i, _status_led[i].state);
    //             }
    //         }
    //         else if (_status_led[i].time == 0)
    //         {
    //             led_control_dischg(batt_lvl, ++_status_led[i].step, i);
    //         }
    //     }

    //     // stop the sequence once all led sequence have been completed. Here, there are three leds
    //     if (step_count == 0x7)
    //     {
    //         led_sequence_action(0);
    //         step_count = 0;
    //     }
    // }
    // else
    // {
    //     for( uint8_t i=0; i<STATUS_LEDS; i++ )
    //     {
    //         // int32_t timer_diff = timerB1_counter-_status_led[i].timer;
    //         if(((_timerB1_counter-_status_led[i].timer) > _status_led[i].time) && (_status_led[i].time != 0) )
    //         {
    //                 led_control_chg(batt_lvl, ++_status_led[i].step, i);
    //                 status_LED_toggler(i, _status_led[i].state);
    //         }
    //         else if (_status_led[i].time == 0)
    //         {
    //             // led_control_chg(batt_lvl, ++_status_led[i].step, i);
    //         }
    //     }
    // }

    if(hasStatus(UPDATING_EEPROM))
    {
        for(uint8_t i=0; i<STATUS_LEDS; i++)
        {
            if((_status_led[i]->getStep()%2) == 0)
            {
                _status_led[i]->setStateTime(led_blink_eeprom[0]);
            }
            else
            {
                _status_led[i]->setStateTime(led_blink_eeprom[1]);
            }
        }
    }
    else if(hasStatus(FAULT))
    {}
    else if(hasStatus(ACTIVE))
    {}
    else if(hasStatus(STANDBY))
    {}
    else if(hasStatus(CHARGING))
    {}
    else if(hasStatus(POWERED_OFF))
    {}

    for (uint8_t i = 0; i < STATUS_LEDS; i++)
    {
        if(_status_led[i].getState() == LOW || (_timer_counter + diff) > _status_led[i].getStateTime())
        {
            _status_led[i].toggle();
        }
    
}
