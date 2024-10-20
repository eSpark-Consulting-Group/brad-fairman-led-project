#ifndef TIMERS
#define TIMERS

#include "timer_interface.h"
#include "tca0.h"
#include "tcb1.h"
#include "status_led.hpp"
#include "output_led.hpp"

class Timer
{
    public:
        Timer(StatusLed* handler)
        : _status_led_handler(handler), _pwm_timer_object(&TCA0_Interface), _status_led_timer_object(&TCB1_Interface)
        {   _period = setFrequency(10000);  }

        void setup();

        void pwmStart();
        void pwmStop();
        void pwmDutySet(uint8_t duty);

        void statusLedStart();
        void statusLedStop();
        void statusLedCountSet(uint16_t count);
    private:
        uint16_t _period = 0xFFFF;              // period value for pwm timer

        TMR_INTERFACE* _pwm_timer_object;//, _status_led_timer_object;
        TMR_INTERFACE* _status_led_timer_object;
        StatusLed* _status_led_handler;
        OutputLed* _output_led_handler;
};

//----------------------------------------------------------------------

uint32_t setFrequency(uint32_t freqInHz);

#endif