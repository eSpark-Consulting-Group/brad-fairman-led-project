/**
  * TCB0 Generated Driver File
  *
  * @file tcb0.c
  *
  * @ingroup tcb0
  *
  * @brief This file contains the API implementation for the TCB0 module driver.
  *
  * @version TCB0 Driver Version 1.1.5
*/
/*
� [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include "tcb0.h"
// #include <avr/io.h>
static long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define DEFAULT_PERIOD_VALUE 0xFF

const struct TMR_INTERFACE TCB0_Interface = {
    .Initialize = TCB0_Initialize,
    .Start = TCB0_Start,
    .Stop = TCB0_Stop,
    .PeriodCountSet = TCB0_Write,
    .TimeoutCallbackRegister = NULL,
    .Tasks = NULL,
    .setDuty = setDutyCycleTCB
};

void (*TCB0_OVF_isr_cb)(void) = NULL;
void (*TCB0_CAPT_isr_cb)(void) = NULL;
void TCB0_CaptureCallbackRegister(TCB0_cb_t cb)
{
	TCB0_CAPT_isr_cb = cb;
}

ISR(TCB0_INT_vect)
{
	/* Insert your TCB interrupt handling code */

	/*
	 *  The Capture interrupt flag is cleared by writing a 1 to it or when the
	 *  Capture register is read in Capture mode
	 */
	if (TCB0.INTFLAGS & TCB_CAPT_bm)
    {
        if (TCB0_CAPT_isr_cb != NULL)
        {
            (*TCB0_CAPT_isr_cb)();
        }

        TCB0.INTFLAGS = TCB_CAPT_bm;
    }

}

void setDutyCycleTCB(uint16_t duty)
{
  // uint16_t cmp_register = TCB0.CCMP;
  // uint16_t low_byte = map(duty, 0, 100, 0, DEFAULT_PERIOD_VALUE);
  // cmp_register |= (low_byte & 0X00FF);
  // TCB0.CCMP = cmp_register;
  TCB0.CCMPH = map(duty, 0, 100, 0, DEFAULT_PERIOD_VALUE);
}

void TCB0_Initialize(void)
{
    // CCMP 3333;
    // TCB0.CCMP = (F_CPU / 1000);//0xD05;
    TCB0.CCMPL = DEFAULT_PERIOD_VALUE;//(F_CPU / 1000);            // Period control
    setDutyCycleTCB(50);
    // PORTC.DIRSET |= PIN0_bm; // Set PC0 as output
    PORTMUX.CTRLD |= 0x01;//PORTMUX_TCB0_bm;       // enable alternate pin for TCB0 WC0

    // CNT undefined; 
    TCB0.CNT = 0x0;

    //ASYNC disabled; CCMPEN enabled; CCMPINIT disabled; CNTMODE INT; 8bit PWM mode; 
    // TCB0.CTRLB = 0x0;
    TCB0.CTRLB = 0x10 | TCB_CNTMODE_PWM8_gc;
    
    //DBGRUN enabled; 
    TCB0.DBGCTRL = 0x1;

    //CAPTEI disabled; EDGE disabled; FILTER disabled; 
    TCB0.EVCTRL = 0x0;

    //CAPT enabled; 
    TCB0.INTCTRL = 0x1;

    //CAPT disabled; 
    TCB0.INTFLAGS = 0x0;

    //Temporary Value
    TCB0.TEMP = 0x0;

    //CLKSEL CLKDIV1; ENABLE enabled; RUNSTDBY disabled; SYNCUPD disabled; 
    TCB0.CTRLA = 0x1;
    TCB0.CTRLA |= TCB_ENABLE_bm;
}

void TCB0_Start(void)
{
    TCB0.CTRLA |= TCB_ENABLE_bm; /* Start Timer */
}

void TCB0_Stop(void)
{
    TCB0.CTRLA &= ~TCB_ENABLE_bm; /* Stop Timer */
}

void TCB0_Write(uint16_t timerVal)
{
    TCB0.CNT = timerVal;
}

uint16_t TCB0_Read(void)
{
    uint16_t readVal;

    readVal = TCB0.CNT;

    return readVal;
}

void TCB0_EnableCaptInterrupt(void)
{
    TCB0.INTCTRL |= TCB_CAPT_bm; /* Capture or Timeout: enabled */
}


void TCB0_DisableCaptInterrupt(void)
{
    TCB0.INTCTRL &= ~TCB_CAPT_bm; /* Capture or Timeout: disabled */
}

inline void TCB0_ClearCaptInterruptFlag(void)
{
    TCB0.INTFLAGS |= TCB_CAPT_bm;
}

inline bool TCB0_IsCaptInterruptFlag(void)
{
	return TCB0.INTFLAGS & TCB_CAPT_bm;

}

inline bool TCB0_IsCaptInterruptEnabled(void)
{
    return ((TCB0.INTCTRL & TCB_CAPT_bm) > 0);
}

