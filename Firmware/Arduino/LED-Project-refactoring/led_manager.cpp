#include "Libraries/LED_Manager/led_manager.hpp"

#define LED_BLINK_STEPS_DISCHG 3
uint32_t led1_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0}, {1000, 200, 2000},  {3000,0,0}, {3000,0,0}, {3000,0,0}};
uint32_t led2_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0}, {500,500,500},            {0,0,0},    {3000,0,0}, {3000,0,0}};
uint32_t led3_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0}, {3000,1000,5000},            {0,0,0},    {0,0,0},    {3000,0,0}};

#define LED_BLINK_STEPS_CHG 2
uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0}, {1000, 1000}, {1000,1},      {1000,1},       {1000,1}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0}, {0,0},        {1000,1000},   {1000,1},       {1000,1}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0}, {0,0},        {0,0},         {1000,1000},    {1000,1}};

#define LED_BLINK_EEPROM_UPDATE 2
uint32_t led_blink_eeprom[LED_BLINK_EEPROM_UPDATE]       = {50, 50};

#define ALL_STATUS_LED_SEQUENCE_COMPLETE    (BIT_SET(0) | BIT_SET(1) | BIT_SET(2))
//----------------------------------------------------------------------

void StatusLeds::toggle()
{
    _control.state = (_control.state == LOW) ? HIGH : LOW;
    updateState();
}

void StatusLeds::on()
{
    _control.state = (STATUS_LEDS_INVERTED) ? LOW : HIGH;
    updateState();
}

void StatusLeds::off()
{
    _control.state = (STATUS_LEDS_INVERTED) ? HIGH : LOW;
    updateState();
}

void StatusLeds::updateState()
{
    digitalWrite(_pin, _control.state);
    _control.step++;
    _control.stateStartTime = millis();
}

//----------------------------------------------------------------------

StatusManager* StatusManager::_instance = nullptr;

StatusManager* StatusManager::getInstance()
{
    if (!_instance) {
        _instance = new StatusManager();
    }
    return _instance;
}

void StatusManager::addLed(StatusLeds* led)
{
    if (_registered_leds < STATUS_LEDS)
    {
        _status_led[_registered_leds++] = led;
    }
}

void StatusManager::ledOn(uint8_t led)
{
    if((led > _registered_leds))
        return;
    
    _status_led[led]->on();
}

void StatusManager::ledOff(uint8_t led)
{
    if((led > _registered_leds))
        return;
    
    _status_led[led]->off();
}

void StatusManager::managerTest(DEVICE_STATE_e testStatus, BATT_LVL_e lvl)
{
    switch(testStatus)
    {
        case DISCHARGING:
            _sequence_ptr[0] = &led1_dischg_sequence;
            _sequence_ptr[1] = &led2_dischg_sequence;
            _sequence_ptr[2] = &led3_dischg_sequence;
            _step_counts = LED_BLINK_STEPS_DISCHG;
            switch(lvl)
            {
                case LVL_20:
                    for (uint8_t i = 0; i < STATUS_LEDS; i++)
                    {
                        _status_led[i]->setStateTime((*_sequence_ptr[i])[1][0]);
                        _status_led[i]->setStep(0);
                        if(0 != _status_led[i]->getStateTime())
                        {
                            _status_led[i]->toggle();
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
    _timer_counter = millis();
}

void StatusManager::registerEvent(EVENTS_LIST_e eventID, uint8_t val)
{
    // switch(eventID)
    // {
    //     case DEVICE_STATUS_UPDATE:
    //         setStatus((DEVICE_STATE_e)val);
    //         switch(val)
    //         {
    //             case FAULT:
    //                 break;
    //             case ACTIVE:
    //                 // start status led timer here
    //                 break;
    //             case STANDBY:
    //                 // stop status led timer here
    //                 break;
    //             case CHARGING:
    //                 _sequence_ptr[0] = &led1_chg_sequence;
    //                 _sequence_ptr[1] = &led2_chg_sequence;
    //                 _sequence_ptr[2] = &led3_chg_sequence;
    //                 _step_counts = LED_BLINK_STEPS_CHG;
    //                 break;
    //             case DISCHARGING:
    //                 _sequence_ptr[0] = &led1_dischg_sequence;
    //                 _sequence_ptr[1] = &led2_dischg_sequence;
    //                 _sequence_ptr[2] = &led3_dischg_sequence;
    //                 _step_counts = LED_BLINK_STEPS_DISCHG;
    //                 break;
    //             case UPDATING_EEPROM:
    //                 _sequence_ptr[0] = &led_blink_eeprom;
    //                 _sequence_ptr[1] = &led_blink_eeprom;
    //                 _sequence_ptr[2] = &led_blink_eeprom;
    //                 _step_counts = LED_BLINK_EEPROM_UPDATE;
    //                 break;
    //         }
    //         break;
    //     case BATTERY_UPDATE_EVENT:
    //         BattFlag((BATT_LVL_e)val);
    //         break;
    //     default:
    //         break;
    // }
}

void StatusManager::statusLedHandler()
{
    unsigned long time = millis();
    unsigned long diff = time - _timer_counter;

    if (diff < 10)
    {
        return;
    }

    _timer_counter = time;

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
    else if(hasStatus(DISCHARGING))
    {
        for(uint8_t i=0; i<_registered_leds; i++)
        {
            if(_status_led[i]->getStep() > _step_counts)
            {
                _bit_flag |= BIT_SET(i);
            }
        }
    }
    else if(hasStatus(POWERED_OFF))
    {}

    for (uint8_t i = 0; i < STATUS_LEDS; i++)
    {
        if( (0 != _status_led[i]->getStateTime()) && 
            ((_timer_counter - _status_led[i]->getStartTime()) > _status_led[i]->getStateTime()) &&
            !(BIT_SET(i) & _bit_flag) )
        {
            _status_led[i]->setStateTime((*_sequence_ptr[i])[1][_status_led[i]->getStep()]);
            _status_led[i]->toggle();
        }
    }
    
    if(_bit_flag & ALL_STATUS_LED_SEQUENCE_COMPLETE)
    {
        // disable status led timer here
    }
}

void StatusManager::statusLedHandlerWrapper()
{
    if(_instance)
    {
        _instance->statusLedHandler();
    }
}
