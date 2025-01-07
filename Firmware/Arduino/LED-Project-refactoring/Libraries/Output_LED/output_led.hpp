#ifndef OUTPUTLED
#define OUTPUTLED

#include "../../definitions.h"

class OutputLed
{
    public:
        OutputLed(uint8_t redPin, uint8_t whitePin)
        : _redLedPin(redPin), _whiteLedPin(whitePin)
        {   pinMode(_redLedPin, OUTPUT);
            pinMode(_whiteLedPin, OUTPUT);
            digitalWrite(_redLedPin, LOW);
            digitalWrite(_whiteLedPin, LOW);     }

        void setModeVar(MODE_e* var)    {   _mode = var;    }
        MODE_e getMode()    {   return *_mode;   }
        void setMode();
        static void ledOn();
        static void ledOff();
    private:
        uint8_t _redLedPin, _whiteLedPin;
        MODE_e* _mode;
};

#endif