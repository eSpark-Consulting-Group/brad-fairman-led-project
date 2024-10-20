#include "status_led.hpp"

#define LED_BLINK_STEPS_DISCHG 3
#define LED_BLINK_STEPS_CHG 2
uint32_t led1_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{1000, 1000, 1000}, {3000,0,0}, {3000,0,0}, {3000,0,0}};
uint32_t led2_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {3000,0,0}, {3000,0,0}};
uint32_t led3_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {0,0,0},    {3000,0,0}};

uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{1000, 1000}, {1000,1},      {1000,1},       {1000,1}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {1000,1000},   {1000,1},       {1000,1}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {0,0},         {1000,1000},    {1000,1}};

void Leds::toggle()
{
    _control.state = !_control.state;
    updateState();
}

void Leds::on()
{
    _control.state = HIGH;
    updateState();
}

void Leds::off()
{
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

StatusLed* StatusLed::instance = nullptr;

static StatusLed* StatusLed::getInstance()
{
    if (!instance) {
        instance = new StatusLed();
    }
    return instance;
}

void StatusLed::addLeds(Leds* led1, Leds* led2, Leds* led3)
{
    _status_led[0] = led1;
    _status_led[1] = led2;
    _status_led[2] = led3;
}

void StatusLed::ledOn(uint8_t led)
{
    if((led >= STATUS_LEDS) || (led == 0))
        return;
    
    _status_led[led-1]->on();
}

void StatusLed::ledOff(uint8_t led)
{
    if((led >= STATUS_LEDS) || (led == 0))
        return;
    
    _status_led[led-1].off();
}

static void StatusLed::statusLedHandler()
{
    _timerB1_counter++;
    static int step_count = 0;

    if(!is_usb_connected)
    {
        for( uint8_t i=0; i<STATUS_LEDS; i++ )
        {
            if(((_timerB1_counter-_status_led[i]->getTimer()) >= _status_led[i]->getTime()) && !(step_count & (1 << i) ))
            {
                if (_status_led[i]->getStep() == (LED_BLINK_STEPS_DISCHG-1) || (_status_led[i]->getTime() == 0))
                {
                    step_count |= (1 << i);
                }
                else
                {
                    led_control_dischg(batt_lvl, ++_status_led[i]->getStep(), i);
                    status_LED_toggler(i, _status_led[i].state);
                }
            }
            else if (_status_led[i].time == 0)
            {
                led_control_dischg(batt_lvl, ++_status_led[i].step, i);
            }
        }

        // stop the sequence once all led sequence have been completed. Here, there are three leds
        if (step_count == 0x7)
        {
            led_sequence_action(0);
            step_count = 0;
        }
    }
    else
    {
        for( uint8_t i=0; i<STATUS_LEDS; i++ )
        {
            // int32_t timer_diff = timerB1_counter-_status_led[i].timer;
            if(((_timerB1_counter-_status_led[i].timer) > _status_led[i].time) && (_status_led[i].time != 0) )
            {
                    led_control_chg(batt_lvl, ++_status_led[i].step, i);
                    status_LED_toggler(i, _status_led[i].state);
            }
            else if (_status_led[i].time == 0)
            {
                // led_control_chg(batt_lvl, ++_status_led[i].step, i);
            }
        }
    }
}
