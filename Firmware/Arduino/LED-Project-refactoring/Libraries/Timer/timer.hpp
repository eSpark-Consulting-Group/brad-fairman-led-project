#ifndef TIMERS
#define TIMERS

#include "timer_interface.h"
#include "tca0.h"
#include "tcb1.h"
#include "../Status_LED/status_led.hpp"
#include "../Output_LED/output_led.hpp"

uint32_t setFrequency(uint32_t freqInHz);

//----------------------------------------------------------------------

class Timer
{
    public:
        Timer(StatusLed* handler)
        : _pwm_timer_object(const_cast<TMR_INTERFACE*>(&TCA0_Interface)), _status_led_timer_object(const_cast<TMR_INTERFACE*>(&TCB1_Interface))
        {   _status_led_handler = handler;
            _period = setFrequency(10000);  }

        void setup();

        void pwmStart();
        void pwmStop();
        void pwmDutySet(uint8_t duty);

        void statusLedStart();
        void statusLedStop();
        void statusLedCountSet(uint16_t count);
    private:
        uint16_t _period = 0xFFFF;              // period value for pwm timer

        TMR_INTERFACE* _pwm_timer_object = NULL;
        TMR_INTERFACE* _status_led_timer_object = NULL;
        StatusLed* _status_led_handler = NULL;
        OutputLed* _output_led_handler = NULL;
};

#endif