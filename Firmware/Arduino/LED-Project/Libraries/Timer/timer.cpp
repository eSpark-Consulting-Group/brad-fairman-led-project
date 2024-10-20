#include "timer.hpp"

void Timer::setup()
{
    _pwm_timer_object->Initialize();
    _status_led_timer_object->Initialize();
    pwmStop();
    statusLedStop();

    _pwm_timer_object->PeriodCountSet(_period);
    _status_led_timer_object->TimeoutCallbackRegister(&_status_led_handler->statusLedHandler());
    _pwm_timer_object->TimeoutCallbackRegister(&_output_led_handler->ledOn());         // overflow
    _pwm_timer_object->CompareCallbackRegister(&_output_led_handler->ledOff());        // compare
}

void Timer::pwmStart()
{
    _pwm_timer_object->Start();
}

void Timer::pwmStop()
{
    _pwm_timer_object->Stop();
}

void Timer::pwmDutySet(uint8_t duty)
{
    _pwm_timer_object->CompareRegisterSet(map(duty, 0, 100, 0, _period));
}

void Timer::statusLedStart()
{
    _status_led_timer_object->Start();
}

void Timer::statusLedStop()
{
    _status_led_timer_object->Stop();
}

void Timer::statusLedCountSet(uint16_t count)
{
    _status_led_timer_object->PeriodCountSet(count);
}

//----------------------------------------------------------------------

uint32_t setFrequency(uint32_t freqInHz)
{
    uint32_t tempperiod = (F_CPU / freqInHz);
    uint8_t presc = 0;
    while (tempperiod > 65536 && presc < 7)
    {
        presc++;
        tempperiod      = tempperiod >> (presc > 4 ? 2 : 1);
    }
    // Period            = tempperiod;
    // TCA0.SINGLE.CTRLA = (presc << 1) | TCA_SINGLE_ENABLE_bm;      // set the prescaler for the timer
    // TCA0.SINGLE.PER   = Period;                                   // set the period
    return tempperiod
}