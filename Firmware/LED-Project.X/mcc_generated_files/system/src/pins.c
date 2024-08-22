/**
 * Generated Driver File
 * 
 * @file pins.c
 * 
 * @ingroup  pinsdriver
 * 
 * @brief This is generated driver implementation for pins. 
 *        This file provides implementations for pin APIs for all pins selected in the GUI.
 *
 * @version Driver Version 1.1.0
*/

/*
© [2024] Microchip Technology Inc. and its subsidiaries.

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

#include "../pins.h"

static void (*IO_PB2_InterruptHandler)(void);
static void (*IO_PB0_InterruptHandler)(void);
static void (*IO_PB1_InterruptHandler)(void);
static void (*GPOUT_InterruptHandler)(void);
static void (*BAT_CHG_InterruptHandler)(void);
static void (*U_INT_InterruptHandler)(void);
static void (*M_BUTTON_InterruptHandler)(void);
static void (*B_HIGH_InterruptHandler)(void);
static void (*B_MID_InterruptHandler)(void);
static void (*B_LOW_InterruptHandler)(void);
static void (*U_KILL_InterruptHandler)(void);
static void (*RED_LED_InterruptHandler)(void);
static void (*CTRL_SIG_InterruptHandler)(void);

void PIN_MANAGER_Initialize()
{

  /* OUT Registers Initialization */
    PORTA.OUT = 0x0;
    PORTB.OUT = 0x0;
    PORTC.OUT = 0x0;

  /* DIR Registers Initialization */
    PORTA.DIR = 0x16;
    PORTB.DIR = 0x27;
    PORTC.DIR = 0x5;

  /* PINxCTRL registers Initialization */
    PORTA.PIN0CTRL = 0x0;
    PORTA.PIN1CTRL = 0x0;
    PORTA.PIN2CTRL = 0x0;
    PORTA.PIN3CTRL = 0x0;
    PORTA.PIN4CTRL = 0x0;
    PORTA.PIN5CTRL = 0x3;
    PORTA.PIN6CTRL = 0x0;
    PORTA.PIN7CTRL = 0x0;
    PORTB.PIN0CTRL = 0x0;
    PORTB.PIN1CTRL = 0x0;
    PORTB.PIN2CTRL = 0x0;
    PORTB.PIN3CTRL = 0x0;
    PORTB.PIN4CTRL = 0x3;
    PORTB.PIN5CTRL = 0x0;
    PORTB.PIN6CTRL = 0x0;
    PORTB.PIN7CTRL = 0x0;
    PORTC.PIN0CTRL = 0x0;
    PORTC.PIN1CTRL = 0x1;
    PORTC.PIN2CTRL = 0x0;
    PORTC.PIN3CTRL = 0x0;
    PORTC.PIN4CTRL = 0x0;
    PORTC.PIN5CTRL = 0x0;
    PORTC.PIN6CTRL = 0x0;
    PORTC.PIN7CTRL = 0x0;

  /* PORTMUX Initialization */
    PORTMUX.CTRLA = 0x0;
    PORTMUX.CTRLB = 0x0;
    PORTMUX.CTRLC = 0x0;
    PORTMUX.CTRLD = 0x0;

  // register default ISC callback functions at runtime; use these methods to register a custom function
    IO_PB2_SetInterruptHandler(IO_PB2_DefaultInterruptHandler);
    IO_PB0_SetInterruptHandler(IO_PB0_DefaultInterruptHandler);
    IO_PB1_SetInterruptHandler(IO_PB1_DefaultInterruptHandler);
    GPOUT_SetInterruptHandler(GPOUT_DefaultInterruptHandler);
    BAT_CHG_SetInterruptHandler(BAT_CHG_DefaultInterruptHandler);
    U_INT_SetInterruptHandler(U_INT_DefaultInterruptHandler);
    M_BUTTON_SetInterruptHandler(M_BUTTON_DefaultInterruptHandler);
    B_HIGH_SetInterruptHandler(B_HIGH_DefaultInterruptHandler);
    B_MID_SetInterruptHandler(B_MID_DefaultInterruptHandler);
    B_LOW_SetInterruptHandler(B_LOW_DefaultInterruptHandler);
    U_KILL_SetInterruptHandler(U_KILL_DefaultInterruptHandler);
    RED_LED_SetInterruptHandler(RED_LED_DefaultInterruptHandler);
    CTRL_SIG_SetInterruptHandler(CTRL_SIG_DefaultInterruptHandler);
}

/**
  Allows selecting an interrupt handler for IO_PB2 at application runtime
*/
void IO_PB2_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    IO_PB2_InterruptHandler = interruptHandler;
}

void IO_PB2_DefaultInterruptHandler(void)
{
    // add your IO_PB2 interrupt custom code
    // or set custom function using IO_PB2_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for IO_PB0 at application runtime
*/
void IO_PB0_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    IO_PB0_InterruptHandler = interruptHandler;
}

void IO_PB0_DefaultInterruptHandler(void)
{
    // add your IO_PB0 interrupt custom code
    // or set custom function using IO_PB0_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for IO_PB1 at application runtime
*/
void IO_PB1_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    IO_PB1_InterruptHandler = interruptHandler;
}

void IO_PB1_DefaultInterruptHandler(void)
{
    // add your IO_PB1 interrupt custom code
    // or set custom function using IO_PB1_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for GPOUT at application runtime
*/
void GPOUT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    GPOUT_InterruptHandler = interruptHandler;
}

void GPOUT_DefaultInterruptHandler(void)
{
    // add your GPOUT interrupt custom code
    // or set custom function using GPOUT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for BAT_CHG at application runtime
*/
void BAT_CHG_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    BAT_CHG_InterruptHandler = interruptHandler;
}

void BAT_CHG_DefaultInterruptHandler(void)
{
    // add your BAT_CHG interrupt custom code
    // or set custom function using BAT_CHG_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for U_INT at application runtime
*/
void U_INT_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    U_INT_InterruptHandler = interruptHandler;
}

void U_INT_DefaultInterruptHandler(void)
{
    // add your U_INT interrupt custom code
    // or set custom function using U_INT_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for M_BUTTON at application runtime
*/
void M_BUTTON_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    M_BUTTON_InterruptHandler = interruptHandler;
}

void M_BUTTON_DefaultInterruptHandler(void)
{
    // add your M_BUTTON interrupt custom code
    // or set custom function using M_BUTTON_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for B_HIGH at application runtime
*/
void B_HIGH_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    B_HIGH_InterruptHandler = interruptHandler;
}

void B_HIGH_DefaultInterruptHandler(void)
{
    // add your B_HIGH interrupt custom code
    // or set custom function using B_HIGH_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for B_MID at application runtime
*/
void B_MID_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    B_MID_InterruptHandler = interruptHandler;
}

void B_MID_DefaultInterruptHandler(void)
{
    // add your B_MID interrupt custom code
    // or set custom function using B_MID_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for B_LOW at application runtime
*/
void B_LOW_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    B_LOW_InterruptHandler = interruptHandler;
}

void B_LOW_DefaultInterruptHandler(void)
{
    // add your B_LOW interrupt custom code
    // or set custom function using B_LOW_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for U_KILL at application runtime
*/
void U_KILL_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    U_KILL_InterruptHandler = interruptHandler;
}

void U_KILL_DefaultInterruptHandler(void)
{
    // add your U_KILL interrupt custom code
    // or set custom function using U_KILL_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for RED_LED at application runtime
*/
void RED_LED_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    RED_LED_InterruptHandler = interruptHandler;
}

void RED_LED_DefaultInterruptHandler(void)
{
    // add your RED_LED interrupt custom code
    // or set custom function using RED_LED_SetInterruptHandler()
}
/**
  Allows selecting an interrupt handler for CTRL_SIG at application runtime
*/
void CTRL_SIG_SetInterruptHandler(void (* interruptHandler)(void)) 
{
    CTRL_SIG_InterruptHandler = interruptHandler;
}

void CTRL_SIG_DefaultInterruptHandler(void)
{
    // add your CTRL_SIG interrupt custom code
    // or set custom function using CTRL_SIG_SetInterruptHandler()
}
ISR(PORTA_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTA.INTFLAGS & PORT_INT5_bm)
    {
       GPOUT_InterruptHandler(); 
    }
    if(VPORTA.INTFLAGS & PORT_INT6_bm)
    {
       BAT_CHG_InterruptHandler(); 
    }
    if(VPORTA.INTFLAGS & PORT_INT1_bm)
    {
       B_HIGH_InterruptHandler(); 
    }
    if(VPORTA.INTFLAGS & PORT_INT2_bm)
    {
       B_MID_InterruptHandler(); 
    }
    if(VPORTA.INTFLAGS & PORT_INT4_bm)
    {
       B_LOW_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTA.INTFLAGS = 0xff;
}

ISR(PORTB_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTB.INTFLAGS & PORT_INT2_bm)
    {
       IO_PB2_InterruptHandler(); 
    }
    if(VPORTB.INTFLAGS & PORT_INT0_bm)
    {
       IO_PB0_InterruptHandler(); 
    }
    if(VPORTB.INTFLAGS & PORT_INT1_bm)
    {
       IO_PB1_InterruptHandler(); 
    }
    if(VPORTB.INTFLAGS & PORT_INT4_bm)
    {
       U_INT_InterruptHandler(); 
    }
    if(VPORTB.INTFLAGS & PORT_INT5_bm)
    {
       U_KILL_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTB.INTFLAGS = 0xff;
}

ISR(PORTC_PORT_vect)
{ 
    // Call the interrupt handler for the callback registered at runtime
    if(VPORTC.INTFLAGS & PORT_INT1_bm)
    {
       M_BUTTON_InterruptHandler(); 
    }
    if(VPORTC.INTFLAGS & PORT_INT0_bm)
    {
       RED_LED_InterruptHandler(); 
    }
    if(VPORTC.INTFLAGS & PORT_INT2_bm)
    {
       CTRL_SIG_InterruptHandler(); 
    }
    /* Clear interrupt flags */
    VPORTC.INTFLAGS = 0xff;
}

/**
 End of File
*/