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

#define DEBOUNCE_TIME   20
#define FUEL_CHECK_TIME 2000            // button press time to trigger batt fuel check
#define MODE_CHANGE_TIME 250            // button press time to trigger mode change
#define EEPROM_Address  0x14FE
#define BATTERY_LVL_MIN 3.0f
#define BATTERY_LVL_MAX 4.2f



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


typedef enum {      // white LED state
    MODE_1 = 1,
    MODE_2,
    MODE_3,
    MODE_4
}MODE_e;

typedef enum {      // batt lvls
    LVL_20,
    LVL_33,
    LVL_66,
    LVL_100,
    LVL_MAX
}BATT_LVL_e;

typedef enum {         // int gpios
    BUTTON_INT,
    U_INT,
    BATT_CHG,
    GPOUT,
}INT_PINS_e;

typedef struct {
    bool state;             // led on or off
    uint8_t step;
    uint16_t time;          // time period for this state
    uint16_t timer;         // when this sequence started
}LED_STATE_t;

LED_STATE_t LED[3];

#define LED_BLINK_STEPS_DISCHG 3
#define LED_BLINK_STEPS_CHG 2
uint32_t led1_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{1000, 1000, 1000}, {3000,0,0}, {3000,0,0}, {3000,0,0}};
uint32_t led2_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {3000,0,0}, {3000,0,0}};
uint32_t led3_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {0,0,0},    {3000,0,0}};
uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{1000, 1000}, {1000,0},      {1000,0},       {1000,0}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0},        {1000,1000},   {1000,0},       {1000,0}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0},        {0,0},         {1000,1000},    {1000,0}};
//uint32_t led_timer = 0;
//uint8_t led_step = 0;

eeprom_data_t current_mode;
volatile BATT_LVL_e batt_lvl = LVL_20;
volatile uint32_t timerB1_counter=0;
volatile uint32_t timerB0_counter=0;
volatile uint32_t debounce_timer=0;
volatile uint32_t button_press_time=0;
volatile bool is_button_pressed = false;
volatile bool is_usb_connected=false;

volatile bool batt_lvl_acquire_flag=false;
volatile bool mode_change_flag=false;


void digitalWrite(uint8_t led, bool state)
{
    if(state)
    {
        switch(led)
        {
            case 0:
                B_HIGH_SetHigh();
                break;
            case 1:
                B_MID_SetHigh();
                break;
            case 2:
                B_LOW_SetHigh();
                break;
            case 3:
                U_KILL_SetHigh();
                break;
        }
    }
    else
    {
        switch(led)
        {
            case 0:
                B_HIGH_SetLow();
                break;
            case 1:
                B_MID_SetLow();
                break;
            case 2:
                B_LOW_SetLow();
                break;
            case 3:
                U_KILL_SetLow();
                break;
        }
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
        current_mode = MODE_1;
        new_mode = current_mode;
    }
    if(EEPROM_Read(EEPROM_Address) != new_mode)
    {
        EEPROM_Write(EEPROM_Address, new_mode);
    }
    update_led(new_mode);
}

void TCA0_cmpr2_isr()
{
   CTRL_SIG_SetLow();
}
void TCA0_overflow_isr()
{
    CTRL_SIG_SetHigh();
}

// bool debounce(INT_PINS_e pin) {
//   static uint16_t state = 0;
//   switch(pin)
//   {
//     case BUTTON_INT:
//         state = (state<<1) | M_BUTTON_GetValue() | 0xfe00;
//         break;
//   }
//   return (state == 0xff00);
// }

// void button_int()
// {
//     if(M_BUTTON_GetValue() && !is_button_pressed)
//     {
//         // TCB0_Start();
//         button_press_time = TCB0_Read();
//         is_button_pressed = true;
//     }
//     else
//     {
//         // TCB0_Stop();
//         uint16_t button_pressed_time_val = TCB0_Read() - button_press_time;

//         if((button_pressed_time_val >= (FUEL_CHECK_TIME-150)) && (button_pressed_time_val <= (FUEL_CHECK_TIME+150)))
//         {
//             batt_lvl_acquire_flag = true;
//         }
//         else if(( button_pressed_time_val >= (MODE_CHANGE_TIME-25)) && (button_pressed_time_val <= (MODE_CHANGE_TIME+25)))
//         {
//             mode_change_flag = true;
//         }
// //        else if()
//         button_press_time = 0;
//     }
// }

void led_control_dischg(BATT_LVL_e lvl, uint8_t step, uint8_t led)
{
    LED[led].state = ((step+1) % 2);
    LED[led].timer = timerB1_counter;//TCB1_Read();
    LED[led].step = step;
    switch(led)
    {
        case 0:
            LED[led].time = led1_dischg_sequence[lvl][step];
            break;
        case 1:
            LED[led].time = led2_dischg_sequence[lvl][step];
            break;
        case 2:
            LED[led].time = led3_dischg_sequence[lvl][step];
            break;
    }

    if(step == 0)
    {
        digitalWrite(led, LED[led].state);
    }
}

void led_sequence_action(bool state)
{
    if(state)
    {
        TCB1_Write(0);
        timerB1_counter = 0;
        TCB1_Start();
    }
    else
    {
        TCB1_Stop();
        // turn off all status LEDs
        digitalWrite(0,0);
        digitalWrite(1,0);
        digitalWrite(2,0);
    }
}

void TCB0_handler()
{
    /* Insert your TCB interrupt handling code */
    timerB0_counter++;

    if((timerB0_counter-debounce_timer) > DEBOUNCE_TIME)     // perform gpio check every 10ms
    {
        debounce_timer = timerB0_counter;
        if(M_BUTTON_GetValue() && !is_button_pressed)
        {
            is_button_pressed = true;
            button_press_time = timerB0_counter;
        }
        else if(!M_BUTTON_GetValue())
        {
            is_button_pressed = false;
            uint16_t button_pressed_time_val = timerB0_counter - button_press_time;

            if((button_pressed_time_val >= (FUEL_CHECK_TIME-200)) && (button_pressed_time_val <= (FUEL_CHECK_TIME+200)))
            {
                batt_lvl_acquire_flag = true;
            }
            else if(( button_pressed_time_val >= (MODE_CHANGE_TIME-150)) && (button_pressed_time_val <= (MODE_CHANGE_TIME+150)))
            {
                mode_change_flag = true;
            }
        }
        
    }

    if(GPOUT_GetValue())            // check batt fuel guage alert pin state
    {

    }

    if(!BAT_CHG_GetValue())            // check if usb is connected, ACTIVE LOW
    {
        is_usb_connected = true;
    }
    else
    {
        is_usb_connected = false;
    }
    
/*
    *  The Capture interrupt flag is cleared by writing a 1 to it or when the
    *  Capture register is read in Capture mode
    */
    if (TCB0.INTFLAGS & TCB_CAPT_bm)
    {
        TCB0.INTFLAGS = TCB_CAPT_bm;
    }

}

void status_LED_handler()
{
    timerB1_counter++;
    static int step_count = 0;

    for( uint8_t i=0; i<3; i++ )
    {
        if(((timerB1_counter-LED[i].timer) >= LED[i].time) && !(step_count & (1 << i) ))
        {
            if (LED[i].step == LED_BLINK_STEPS_DISCHG)
            {
                step_count |= (1 << i);
            }
            else{
                led_control_dischg(batt_lvl, ++LED[i].step, i);
                digitalWrite(i, LED[i].state);
            }
        }
        else if (LED[i].time == 0)
        {
            led_control_dischg(batt_lvl, ++LED[i].step, i);
        }
    }

    if (step_count == 0x7)
    {
        led_sequence_action(0);
        step_count = 0;
    }

}

float mapValue(float *input) {
    return ((*input - BATTERY_LVL_MIN) * (100.0 - 0.0)) / (BATTERY_LVL_MAX - BATTERY_LVL_MIN);
}

BATT_LVL_e batt_lvl_percent(float *batt_voltage)
{
    float soc = mapValue(batt_voltage);

    if(soc<20.0)
    {
        return LVL_20;
    }
    else if(soc<33.0)
    {
        return LVL_33;
    }
    else if(soc<66.0)
    {
        return LVL_66;
    }
    else if(soc<100.0)
    {
        return LVL_100;
    }
    return LVL_MAX;
}

void batt_fuel_ic_init()
{
    SFE_MAX1704X_Init(MAX1704X_MAX17048);
    while(!SFE_MAX1704X_begin()) {}
    SFE_MAX1704X_quickStart();
    SFE_MAX1704X_setThreshold(4);
}

int main(void)
{
    SYSTEM_Initialize();
    // TCB0_Stop();         // 1ms timer for button
    TCB1_Stop();         // 1ms timer for LEDs

    // timer callbacks for PWM generation
    TCA0_Stop();
    TCA0_Compare2CallbackRegister(&TCA0_cmpr2_isr);
    TCA0_OverflowCallbackRegister(&TCA0_overflow_isr);

    TCB0_CaptureCallbackRegister(&TCB0_handler);
    TCB1_CaptureCallbackRegister(&status_LED_handler);

    // button_press_time = 0;
    // M_BUTTON_EnableInterruptForBothEdges();
    // M_BUTTON_SetInterruptHandler(&button_int);
    
    // read last mode setting
    current_mode = EEPROM_Read(EEPROM_Address);
    update_mode(current_mode);
    
    // initialize batt fuel guage sensor
    batt_fuel_ic_init();

    sei();

    while(1)
    {
        if(mode_change_flag)            // if button pressed once change mode accordingly
        {
            current_mode++;
            update_mode(current_mode);
            mode_change_flag = false;
        }

        if(batt_lvl_acquire_flag && !is_usb_connected)       // if button held for 2secs, flash onboard LEDs based on chg lvl
        {
            float current_batt_lvl = SFE_MAX1704X_getVoltage();
            batt_lvl = batt_lvl_percent(&current_batt_lvl);

            led_control_dischg(batt_lvl, 0, 0);
            led_control_dischg(batt_lvl, 0, 1);
            led_control_dischg(batt_lvl, 0, 2);
            led_sequence_action(1);
            batt_lvl_acquire_flag = false;
        }

        if(is_usb_connected)
        {

        }
      
//        RED_LED_SetHigh();
////        led_state = RED_LED_GetValue();
//        DELAY_milliseconds(1000);
//        RED_LED_SetLow();
////        led_state = RED_LED_GetValue();
//        DELAY_milliseconds(1000);
    }    
}