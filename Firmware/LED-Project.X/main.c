 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.2
 *
 * @version Package Version: 3.1.2
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
#include "mcc_generated_files/system/system.h"
#include "mcc_generated_files/timer/delay.h"
#include "Library/MAX17048_BATT_fuel/SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"
/*
    Main application
*/
#define EEPROM_Address 0x14FE
#define FUEL_CHECK_TIME 2000
#define MODE_CHANGE_TIME 100

//void LED_drive(uint16_t duty)
//{
//    CTRL_SIG_SetHigh();
//    DELAY_microseconds(duty*10);
//    CTRL_SIG_SetLow();
//    DELAY_microseconds(1000-duty);
//}
//
//void LED_drive5sec(uint16_t duty)
//{
////    RED_LED_SetHigh();
//    for(int i=0; i<500; i++)
//    {
//        LED_drive(duty);
//    }
////    RED_LED_SetLow();
//}

//uint16_t led_chg_sequence_1[] = {1000, 1000, 1000};
//uint16_t led_chg_sequence_2[] = {3000};
//uint16_t led_chg_sequence_3[];

typedef enum {
    MODE_1 = 1,
    MODE_2,
    MODE_3,
    MODE_4
}MODE_e;

typedef enum {
    LVL_20,
    LVL_33,
    LVL_66,
    LVL_100,
}BATT_LVL_e;

eeprom_data_t current_mode;
uint32_t button_timer=0;
bool batt_lvl_acquire_flag=false;
bool mode_change_flag=false;

ISR(TCB0_INT_vect)
{
	/* Insert your TCB interrupt handling code */
    button_timer++;

	/*
	 *  The Capture interrupt flag is cleared by writing a 1 to it or when the
	 *  Capture register is read in Capture mode
	 */
	if (TCB0.INTFLAGS & TCB_CAPT_bm)
    {
//        if (TCB0_CAPT_isr_cb != NULL)
//        {
//            (*TCB0_CAPT_isr_cb)();
//        }

        TCB0.INTFLAGS = TCB_CAPT_bm;
    }

}

void update_led(MODE_e mode)
{
    TCA0_Stop();
    RED_LED_SetLow();
    switch(mode)
    {
        case MODE_4:
            RED_LED_SetHigh();
            break;
        case MODE_1:
            TCA0_Compare2Set((TCA0.SINGLE.PER)/4);
            TCA0_Start();
            break;
        case MODE_2:
            TCA0_Compare2Set((TCA0.SINGLE.PER)/2);
            TCA0_Start();
            break;
        case MODE_3:
            TCA0_Compare2Set(TCA0.SINGLE.PER-1);
            TCA0_Start();
            break;
    }
}

void update_mode(MODE_e new_mode)
{
    if(new_mode>MODE_4)
    {
        current_mode = MODE_3;
        new_mode = current_mode;
    }
    if(EEPROM_Read(EEPROM_Address) != new_mode)
    {
        EEPROM_Write(EEPROM_Address, new_mode);
    }
    update_led(new_mode);
}

void button_int()
{
    if(M_BUTTON_GetValue())
    {
        TCB0_Start();
    }
    else
    {
        TCB0_Stop();
        if(((FUEL_CHECK_TIME-150)<= button_timer) && (button_timer<=(FUEL_CHECK_TIME+150)))
        {
            batt_lvl_acquire_flag = true;
        }
        else if(((MODE_CHANGE_TIME-25)<= button_timer) && (button_timer<=(MODE_CHANGE_TIME+25)))
        {
            mode_change_flag = true;
        }
        button_timer = 0;
    }
}

int main(void)
{
    SYSTEM_Initialize();
    TCB0_Stop();
    TCA0_Stop();
    button_timer = 0;
    M_BUTTON_EnableInterruptForBothEdges();
    M_BUTTON_SetInterruptHandler(&button_int);
    
//    DELAY_milliseconds(5000);
    
    current_mode = EEPROM_Read(EEPROM_Address);
    update_mode(current_mode);
    
//    SFE_MAX1704X_Init(MAX1704X_MAX17048);
//    bool max_status = SFE_MAX1704X_begin();
//    if(max_status)
//    SFE_MAX1704X_quickStart();
//        B_HIGH_SetHigh();
//        B_MID_SetHigh();
//        B_LOW_SetHigh();
        
//        TCA0_Start();
//        TCA0_EnableInterrupt();
//        uint16_t cntVal = TCA0_Read();
//        if(cntVal){}
        
//        TCA0_Compare2Set();
        sei();
////        bool cmpFlag, ovrFlag;
//        
//        DELAY_milliseconds(1000);
////        TCA0_Stop();
////        TCA0_Compare0Set(0);
////        TCA0_Compare1Set(0);
//        TCA0_Compare2Set(3000);
//        DELAY_milliseconds(1000);
//        TCA0_Compare2Set(200);
//        DELAY_milliseconds(1000);
//        TCA0_Compare2Set(2000);
//        TCA0_Start();
//        uint8_t led_state;
    while(1)

    {
        if(mode_change_flag)
        {
            current_mode++;
            if(current_mode>MODE_4)
            {
                current_mode = MODE_1;
            }
            update_mode(current_mode);
            mode_change_flag = false;
        }
        if(batt_lvl_acquire_flag)
        {
            batt_lvl_acquire_flag = false;
        }
      
//        RED_LED_SetHigh();
////        led_state = RED_LED_GetValue();
//        DELAY_milliseconds(1000);
//        RED_LED_SetLow();
////        led_state = RED_LED_GetValue();
//        DELAY_milliseconds(1000);
    }    
}