#include "Libraries/Timer/timer.hpp"

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

//----------------------------------------------------------------------

void Timer::setup()
{
    _white_pwm_timer_object->Initialize();                // Main WHITE Led PWM generating timer
    _red_pwm_timer_object->Initialize();                  // Main RED Led PWM denerating timer
    _status_led_timer_object->Initialize();               // 1ms timer for Status LEDs sequence
    _white_pwm_timer_object->PeriodCountSet(50000);       // set PWM freq (5 - 100 KHz range)
    // _red_pwm_timer_object->setDuty(50);                   // set red led duty cycle   - For some reason, unable to modify dutyCycle outside of .Initialize()
    
    // pwmStop();
    // statusLedStop();

    _status_led_timer_object->TimeoutCallbackRegister(&_status_led_handler->statusLedHandler());
    // _white_pwm_timer_object->TimeoutCallbackRegister(&_output_led_handler->ledOn());         // overflow
    // _white_pwm_timer_object->CompareCallbackRegister(&_output_led_handler->ledOff());        // compare
}

void Timer::pwmStart()
{
    _white_pwm_timer_object->Start();
}

void Timer::pwmStop()
{
    _white_pwm_timer_object->Stop();
}

void Timer::pwmDutySet(uint8_t duty)
{
    _white_pwm_timer_object->CompareRegisterSet(map(duty, 0, 100, 0, _period));
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
