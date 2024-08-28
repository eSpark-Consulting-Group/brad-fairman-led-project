#include <stdint.h>
#include <Wire.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <EEPROM.h>
#include "tca0.h"
#include "tcb0.h"
#include "tcb1.h"

#define STATUS_LED_75 PIN_PA1
#define STATUS_LED_50 PIN_PA2
#define STATUS_LED_25 PIN_PA4
#define GPOUT_PIN     PIN_PA5
#define BATT_CHG_PIN  PIN_PA6
#define SCL           PIN_PB0
#define SDA           PIN_PB1
#define U_INT_PIN     PIN_PB4
#define U_KILL_PIN    PIN_PB5
#define RED_LED_CTRL  PIN_PC0
#define WHITE_LED_CTRL  PIN_PC2
#define BUTTON_PIN    PIN_PC1

#define DEBOUNCE_TIME   50
#define FUEL_CHECK_TIME 2000            // button press time to trigger batt fuel check
#define MODE_CHANGE_TIME 250            // button press time to trigger mode change
// #define EEPROM_Address  0x00//14FE
#define EEPROM_ENDURANCE  100000        // Number of cycles EEPROM can be erased/written
#define BATTERY_LVL_MIN 3.0f
#define BATTERY_LVL_MAX 4.2f

SFE_MAX1704X lipo(MAX1704X_MAX17048); // Create a MAX17048
EEPROMClass EEPROM1;
uint8_t EEPROM_Address;
uint8_t EEPROM_Write_Count;
uint8_t EEPROM_Mode_Addr;

unsigned int Period = 0xFFFF;

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

typedef struct {
    bool state;             // led on or off
    uint8_t step;
    uint32_t time;          // time period for this state
    uint32_t timer;         // when this sequence started
}LED_STATE_t;

LED_STATE_t LED[3];

#define LED_BLINK_STEPS_DISCHG 3
#define LED_BLINK_STEPS_CHG 2
uint32_t led1_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{1000, 1000, 1000}, {3000,0,0}, {3000,0,0}, {3000,0,0}};
uint32_t led2_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {3000,0,0}, {3000,0,0}};
uint32_t led3_dischg_sequence[LVL_MAX][LED_BLINK_STEPS_DISCHG] = {{0,0,0},            {0,0,0},    {0,0,0},    {3000,0,0}};

uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG] = {{1000, 1000}, {1000,1},      {1000,1},       {1000,1}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG] = {{0,0},        {1000,1000},   {1000,1},       {1000,1}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG] = {{0,0},        {0,0},         {1000,1000},    {1000,1}};

MODE_e current_mode = MODE_1;
volatile BATT_LVL_e batt_lvl = LVL_20;
volatile uint32_t timerB1_counter=0;
volatile uint32_t timerB0_counter=0;
volatile uint32_t debounce_timer=0;
volatile uint32_t button_press_time=0;
volatile bool is_button_pressed = false;
volatile bool fuel_pressed = false;
volatile bool is_usb_connected=false;
volatile BATT_LVL_e chg_sequence = (BATT_LVL_e)5;

volatile bool batt_lvl_acquire_flag=false;
volatile bool mode_change_flag=false;

// ISR for main WHITE/RED LEDs
void TCA0_cmpr2_isr()
{
  if(current_mode!=MODE_4)
  {
    digitalWrite(WHITE_LED_CTRL, LOW);
  }
  else{
    digitalWrite(RED_LED_CTRL, LOW);
  }
}
void TCA0_overflow_isr()
{
  if(current_mode!=MODE_4)
  {
    digitalWrite(WHITE_LED_CTRL, HIGH);
  }
  else{
    digitalWrite(RED_LED_CTRL, HIGH);
  }
}

void setDutyCycleTCA(byte duty) {
  TCA0.SINGLE.CMP2 = map(duty, 0, 100, 0, Period);
}

void update_led(MODE_e mode)
{
    TCA0_Stop();
    digitalWrite(RED_LED_CTRL, LOW);
    digitalWrite(WHITE_LED_CTRL, LOW);
    switch(mode)
    {
        case MODE_4:
            // TCA0_Compare2Set((TCA0.SINGLE.PER)/2);
            setDutyCycleTCA(50);
            TCA0_Start();
            break;
        case MODE_1:
//            TCA0_Compare2Set((TCA0.SINGLE.PER)/4);
            // TCA0_Compare2Set(TCA0.SINGLE.PER*(.7f));
            setDutyCycleTCA(25);
            TCA0_Start();
            break;
        case MODE_2:
            // TCA0_Compare2Set((TCA0.SINGLE.PER)/2);
            setDutyCycleTCA(50);
            TCA0_Start();
            break;
        case MODE_3:
//            TCA0_Compare2Set(TCA0.SINGLE.PER-1);
            // TCA0_Compare2Set((TCA0.SINGLE.PER)/4);
            setDutyCycleTCA(75);
            TCA0_Start();
            break;
    }
}

void update_mode(MODE_e* mode)
{
    if(*mode > MODE_4)
    {
        *mode = MODE_1;
    }

    eeprom_mode_write(*mode);
    update_led(*mode);
}

void setFrequency(unsigned long freqInHz) {
  unsigned long tempperiod = (F_CPU / freqInHz);
  byte presc = 0;
  while (tempperiod > 65536 && presc < 7) {
    presc++;
    tempperiod      = tempperiod >> (presc > 4 ? 2 : 1);
  }
  Period            = tempperiod;
  TCA0.SINGLE.CTRLA = (presc << 1) | TCA_SINGLE_ENABLE_bm;
  TCA0.SINGLE.PER   = Period;
}

void led_control_chg(BATT_LVL_e lvl, uint8_t step, uint8_t led)
{
  LED[led].state = ((step+1) % 2);
  LED[led].timer = timerB1_counter;
  LED[led].step = step%2;
  switch(led)
  {
      case 0:
          LED[led].time = led1_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
              // digitalWrite(STATUS_LED_75, LED[led].state);
              status_LED_toggler(led, LED[led].state);
          }
          break;
      case 1:
          LED[led].time = led2_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
              // digitalWrite(STATUS_LED_50, LED[led].state);
              status_LED_toggler(led, LED[led].state);
          }
          break;
      case 2:
          LED[led].time = led3_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
              // digitalWrite(STATUS_LED_25, LED[led].state);
              status_LED_toggler(led, LED[led].state);
          }
          break;
  }
}

void led_control_dischg(BATT_LVL_e lvl, uint8_t step, uint8_t led)
{
    LED[led].timer = timerB1_counter;
    LED[led].step = step;
    LED[led].state = ((step+1) % 2);
    switch(led)
    {
        case 0:
            LED[led].time = led1_dischg_sequence[lvl][step];
            if((step == 0) && (LED[led].time!=0))
            {
              status_LED_toggler(led, LED[led].state);
            }
            break;
        case 1:
            LED[led].time = led2_dischg_sequence[lvl][step];
            if((step == 0) && (LED[led].time!=0))
            {
              status_LED_toggler(led, LED[led].state);
            }
            break;
        case 2:
            LED[led].time = led3_dischg_sequence[lvl][step];
            if((step == 0) && (LED[led].time!=0))
            {
              status_LED_toggler(led, LED[led].state);
            }
            break;
    }
}

void led_sequence_action(bool state)
{
    if(state)
    {
        TCB1_Write(0);
        timerB1_counter = 0;
        if(!is_usb_connected)
        {
          led_control_dischg(batt_lvl, 0, 0);
          led_control_dischg(batt_lvl, 0, 1);
          led_control_dischg(batt_lvl, 0, 2);
        }
        else
        {
          led_control_chg(batt_lvl, 0, 0);
          led_control_chg(batt_lvl, 0, 1);
          led_control_chg(batt_lvl, 0, 2);
        }
        TCB1_Start();
    }
    else
    {
        TCB1_Stop();
        chg_sequence = (BATT_LVL_e)5;
        // turn off all status LEDs
        for(int i=0; i<3; i++)
        {
          status_LED_toggler(i, 0);
          LED[i].state = false;
          LED[i].step = 0;
          LED[i].time = 0;
          LED[i].timer = 0;
        }
    }
}

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

void TCB0_handler()
{
    /* Insert your TCB interrupt handling code */
    timerB0_counter++;

    // if((timerB0_counter-debounce_timer) > DEBOUNCE_TIME)     // perform gpio check every 10ms
    // {
    //     debounce_timer = timerB0_counter;
    //     if(digitalRead(BUTTON_PIN) && !is_button_pressed)
    //     {
    //         is_button_pressed = true;
    //         button_press_time = timerB0_counter;
    //     }
    //     else if(!digitalRead(BUTTON_PIN))
    //     {
    //         is_button_pressed = false;
    //         uint16_t button_pressed_time_val = timerB0_counter - button_press_time;

    //         if((button_pressed_time_val >= (FUEL_CHECK_TIME-200)) && (button_pressed_time_val <= (FUEL_CHECK_TIME+200)))
    //         {
    //             batt_lvl_acquire_flag = true;
    //         }
    //         else if(( button_pressed_time_val >= (MODE_CHANGE_TIME-150)) && (button_pressed_time_val <= (MODE_CHANGE_TIME+150)))
    //         {
    //             mode_change_flag = true;
    //         }
    //     }
        
    // }

    // if(digitalRead(GPOUT_PIN))            // check batt fuel guage alert pin state
    // {

    // }

    // if(!digitalRead(BATT_CHG_PIN))            // check if usb is connected, ACTIVE LOW
    // {
    //     is_usb_connected = true;
    // }
    // else if(is_usb_connected)
    // {
    //     led_sequence_action(0);
    //     is_usb_connected = false;
    // }
}

void status_LED_toggler(int led_num, bool state)
{
  switch(led_num)
  {
    case 0:
      digitalWrite(STATUS_LED_75, state);
      break;
    case 1:
      digitalWrite(STATUS_LED_50, state);
      break;
    case 2:
      digitalWrite(STATUS_LED_25, state);
      break;
  }
}

void status_LED_handler()
{
    timerB1_counter++;
    static int step_count = 0;

    if(!is_usb_connected)
    {
      for( uint8_t i=0; i<3; i++ )
      {
          if(((timerB1_counter-LED[i].timer) >= LED[i].time) && !(step_count & (1 << i) ))
          {
              if (LED[i].step == (LED_BLINK_STEPS_DISCHG-1) || (LED[i].time == 0))
              {
                  step_count |= (1 << i);
              }
              else{
                  led_control_dischg(batt_lvl, ++LED[i].step, i);
                  status_LED_toggler(i, LED[i].state);
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
    else
    {
      for( uint8_t i=0; i<3; i++ )
      {
          // int32_t timer_diff = timerB1_counter-LED[i].timer;
          if(((timerB1_counter-LED[i].timer) > LED[i].time) && (LED[i].time != 0) )
          {
                led_control_chg(batt_lvl, ++LED[i].step, i);
                status_LED_toggler(i, LED[i].state);
          }
          else if (LED[i].time == 0)
          {
              // led_control_chg(batt_lvl, ++LED[i].step, i);
          }
      }
    }
}

float mapValue(float *input) {
    return ((*input - BATTERY_LVL_MIN) * (100.0 - 0.0)) / (BATTERY_LVL_MAX - BATTERY_LVL_MIN);
}

BATT_LVL_e batt_lvl_percent(float *batt_voltage)
{
    float soc = mapValue(batt_voltage);

    if(soc<=20.0)
    {
        return LVL_20;
    }
    else if(soc<=33.0)
    {
        return LVL_33;
    }
    else if(soc<=66.0)
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
    while(!lipo.begin()) {
    }
    lipo.quickStart();
    lipo.setThreshold(4);
    lipo.disableSOCAlert();
}

void eeprom_setup()
{
  EEPROM_Write_Count = EEPROM1.read(EEPROM1.begin());
  EEPROM_Address = EEPROM1.begin()+1;
  EEPROM_Mode_Addr = (MODE_e)(EEPROM1.read(EEPROM_Address));

  current_mode = (MODE_e)(EEPROM1.read(EEPROM_Mode_Addr));
  if(current_mode > MODE_4)
  {
    current_mode = MODE_1;
    eeprom_mode_write(current_mode);
  }
}

void eeprom_mode_write(MODE_e mode)
{
  EEPROM_Write_Count++;
  if(EEPROM_Write_Count > (EEPROM_ENDURANCE-100))
  {
    EEPROM_Write_Count=1;
    EEPROM_Mode_Addr++;
  }
  EEPROM1.update(EEPROM_Address, EEPROM_Mode_Addr);
  EEPROM1.update(EEPROM_Mode_Addr, mode);
  EEPROM1.update(EEPROM1.begin(), EEPROM_Write_Count);
}

void setup()
{
  // put your setup code here, to run once:
  pinMode(STATUS_LED_75, OUTPUT);
  pinMode(STATUS_LED_50, OUTPUT);
  pinMode(STATUS_LED_25, OUTPUT);
  pinMode(U_KILL_PIN, OUTPUT);
  pinMode(RED_LED_CTRL, OUTPUT);
  pinMode(WHITE_LED_CTRL, OUTPUT);

  pinMode(U_INT_PIN, INPUT);
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GPOUT_PIN, INPUT);
  pinMode(BATT_CHG_PIN, INPUT);

  // update EEPROM with initial setup
  // i.e. addr 0 is for number of times EEPROM_Mode_addr have been written
  //      addr 1 holds the address of mode addr
  // 
  // // for(int i=0;i<EEPROM_SIZE;i++)
  // // {
  //      EEPROM.update(i,0);
  // //   digitalWrite(STATUS_LED_75, HIGH);
  // //   delay(10);
  // //   digitalWrite(STATUS_LED_75, LOW);
  // //   delay(10);
  // // }
  //   EEPROM.update(0,100);
  //   EEPROM.update(1,2);
  //   EEPROM.update(2,1);
  // digitalWrite(STATUS_LED_25, HIGH);
  // delay(100);
  // digitalWrite(STATUS_LED_25, LOW);
  // // delay(100);

  // while(1){}


  // takeOverTCA0();             // This replaces disabling and resettng the timer, required previously.
  TCA0_Initialize();
  TCB0_Initialize();           // Timer for checking button/Interrupt pins
  TCB1_Initialize();           // 1ms timer for Status LEDs sequence
  setFrequency(1000);          // set PWM freq to 1KHz
  // // TCB0_Stop();         // 1ms timer for button
  // TCB1_Stop();

  // timer callbacks for PWM generation
  TCA0_Stop();
  TCA0_Compare2CallbackRegister(&TCA0_cmpr2_isr);
  TCA0_OverflowCallbackRegister(&TCA0_overflow_isr);

  TCB0_CaptureCallbackRegister(&TCB0_handler);
  TCB1_CaptureCallbackRegister(&status_LED_handler);

  // uint32_t var1 = timerB0_counter;
  // static bool toggle = false;
  // while(1)
  // {
  //   if((timerB0_counter - var1) > 1000){
  //     toggle = !toggle;
  //     digitalWrite(STATUS_LED_25,toggle);
  //     var1 = timerB0_counter;
  //   }
  // }

  Wire.pins(SDA, SCL);
  Wire.begin();

  batt_fuel_ic_init();

  eeprom_setup();
  update_mode(&current_mode);

  led_sequence_action(0);

}

void loop() {

  if(mode_change_flag && !is_usb_connected)            // if button pressed once change mode accordingly
  {
      current_mode = (current_mode + 1);
      update_mode(&current_mode);
      mode_change_flag = false;
  }
  else
  {
      mode_change_flag = false;
  }

  if(batt_lvl_acquire_flag && !is_usb_connected)       // if button held for 2secs, flash onboard LEDs based on chg lvl
  {
      led_sequence_action(0);
      float current_batt_lvl = lipo.getVoltage();
      batt_lvl = batt_lvl_percent(&current_batt_lvl);
    // batt_lvl = LVL_20;
      led_sequence_action(1);
      batt_lvl_acquire_flag = false;
  }

  if(is_usb_connected)
  {
      // led_sequence_action(0);
    float current_batt_lvl = lipo.getVoltage();
    // float current_batt_lvl = 3.85f;
    batt_lvl = batt_lvl_percent(&current_batt_lvl);
    // batt_lvl = LVL_20;
    if(batt_lvl!=0)
    {
      batt_lvl = batt_lvl-1;
    }
    if(batt_lvl != chg_sequence) {
      led_sequence_action(0);
      chg_sequence = batt_lvl;
      led_sequence_action(1);
    }
  }

  delay(DEBOUNCE_TIME);
  bool state = !digitalRead(BUTTON_PIN) ;
    if(state)//&& debounce_condition)
    {
        if (!is_button_pressed){
          button_press_time = timerB0_counter;
          is_button_pressed = true;
        }
        else
        {
          uint32_t button_pressed_time_val = timerB0_counter - button_press_time;

          if(button_pressed_time_val >= FUEL_CHECK_TIME)
          {
              batt_lvl_acquire_flag = true;
              is_button_pressed = false;
              fuel_pressed = true;
          }
        }
    }
    else
    {
        if(is_button_pressed && (!fuel_pressed))
        {
          uint32_t button_pressed_time_val = timerB0_counter - button_press_time;

          if((button_pressed_time_val >= MODE_CHANGE_TIME) && (button_pressed_time_val < FUEL_CHECK_TIME))
          {
              mode_change_flag = true;
              is_button_pressed = false;
          }
        }
        else{
          fuel_pressed = false;
          is_button_pressed = false;
        }
    }
    
    if(!digitalRead(GPOUT_PIN))            // check batt fuel guage alert pin state
    {
      float current_batt_lvl = lipo.getVoltage();
      if(current_batt_lvl <= BATTERY_LVL_MIN)
      {
        lipo.clearAlert();
        digitalWrite(U_KILL_PIN, HIGH);
      }
    }

    if(!digitalRead(BATT_CHG_PIN))            // check if usb is connected, ACTIVE LOW
    {
        TCA0_Stop();
        setDutyCycleTCA(0);
        is_usb_connected = true;
    }
    else if(is_usb_connected)
    {
        TCA0_Start();
        led_sequence_action(0);
        update_mode(&current_mode);
        is_usb_connected = false;
    }
}
