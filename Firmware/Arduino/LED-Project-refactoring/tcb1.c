/**
  * TCB1 Generated Driver File
  *
  * @file tcb1.c
  *
  * @ingroup tcb1
  *
  * @brief This file contains the API implementation for the TCB1 module driver.
  *
  * @version TCB1 Driver Version 1.1.5
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

#include "Libraries/Timer/tcb1.h"
// #include <avr/io.h>

const struct TMR_INTERFACE TCB1_Interface = {
    .Initialize = TCB1_Initialize,
    .Start = TCB1_Start,
    .Stop = TCB1_Stop,
    .PeriodCountSet = TCB1_Write,
    .TimeoutCallbackRegister = NULL,
    .Tasks = NULL,
    .setDuty = NULL
};

void (*TCB1_OVF_isr_cb)(void) = NULL;
void (*TCB1_CAPT_isr_cb)(void) = NULL;
void TCB1_CaptureCallbackRegister(TCB1_cb_t cb)
{
	TCB1_CAPT_isr_cb = cb;
}


ISR(TCB1_INT_vect)
{
	/* Insert your TCB interrupt handling code */

	/*
	 *  The Capture interrupt flag is cleared by writing a 1 to it or when the
	 *  Capture register is read in Capture mode
	 */
	if (TCB1.INTFLAGS & TCB_CAPT_bm)
    {
        if (TCB1_CAPT_isr_cb != NULL)
        {
            (*TCB1_CAPT_isr_cb)();
        }

        TCB1.INTFLAGS = TCB_CAPT_bm;
    }

}

void TCB1_Initialize(void)
{
    // CCMP 3333; 
    TCB1.CCMP = (F_CPU / 1000);//0xD05;

    // CNT undefined; 
    TCB1.CNT = 0x0;

    //ASYNC disabled; CCMPEN disabled; CCMPINIT disabled; CNTMODE INT; 
    TCB1.CTRLB = 0x0;
    
    //DBGRUN disabled; 
    TCB1.DBGCTRL = 0x0;

    //CAPTEI disabled; EDGE disabled; FILTER disabled; 
    TCB1.EVCTRL = 0x0;

    //CAPT enabled; 
    TCB1.INTCTRL = 0x1;

    //CAPT disabled; 
    TCB1.INTFLAGS = 0x0;

    //Temporary Value
    TCB1.TEMP = 0x0;

    //CLKSEL CLKDIV1; ENABLE enabled; RUNSTDBY disabled; SYNCUPD disabled; 
    TCB1.CTRLA = 0x1;
    TCB1.CTRLA &= ~TCB_ENABLE_bm; /* Stop Timer */
}

void TCB1_Start(void)
{
    TCB1.CTRLA |= TCB_ENABLE_bm; /* Start Timer */
}

void TCB1_Stop(void)
{
    TCB1.CTRLA &= ~TCB_ENABLE_bm; /* Stop Timer */
}

bool TCB1_IsRunning(void)
{
    return (TCB1.CTRLA & TCB_ENABLE_bm) != 0;
}

void TCB1_Write(uint16_t timerVal)
{
    TCB1.CNT = timerVal;
}

uint16_t TCB1_Read(void)
{
    uint16_t readVal;

    readVal = TCB1.CNT;

    return readVal;
}

void TCB1_EnableCaptInterrupt(void)
{
    TCB1.INTCTRL |= TCB_CAPT_bm; /* Capture or Timeout: enabled */
}


void TCB1_DisableCaptInterrupt(void)
{
    TCB1.INTCTRL &= ~TCB_CAPT_bm; /* Capture or Timeout: disabled */
}

inline void TCB1_ClearCaptInterruptFlag(void)
{
    TCB1.INTFLAGS |= TCB_CAPT_bm;
}

inline bool TCB1_IsCaptInterruptFlag(void)
{
	return TCB1.INTFLAGS & TCB_CAPT_bm;

}

inline bool TCB1_IsCaptInterruptEnabled(void)
{
    return ((TCB1.INTCTRL & TCB_CAPT_bm) > 0);
}

