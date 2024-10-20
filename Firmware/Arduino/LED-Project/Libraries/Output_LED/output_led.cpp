#include "output_led.hpp"

void OutputLed::setMode()
{
    if(MODE_OFF == *_mode)
    {
        digitalWrite(_whiteLedPin, LOW);
        digitalWrite(_redLedPin, LOW);
    }
}

static void OutputLed::ledOn()
{
    if( MODE_4 == *_mode )
    {
        digitalWrite(_redLedPin, HIGH);
    }
    else if((MODE_OFF & *_mode) != *_mode )
    {
        digitalWrite(_whiteLedPin, HIGH);
    }
}

static void OutputLed::ledOff()
{
    if( MODE_4 == *_mode )
    {
        digitalWrite(_redLedPin, LOW);
    }
    else if((MODE_OFF & *_mode) != *_mode )
    {
        digitalWrite(_whiteLedPin, LOW);
    }
}
