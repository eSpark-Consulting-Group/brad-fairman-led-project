#include <stdint.h>
#include <Wire.h>
#include <SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h>
#include <EEPROM.h>
#include "tca0.h"
#include "tcb0.h"
#include "tcb1.h"
#include <Arduino.h>

#define STATUS_LED_75   PIN_PA1
#define STATUS_LED_50   PIN_PA2
#define STATUS_LED_25   PIN_PA4
#define GPOUT_PIN       PIN_PA5
#define BATT_CHG_PIN    PIN_PA6
#define SCL             PIN_PB0
#define SDA             PIN_PB1
#define U_INT_PIN       PIN_PB4
#define U_KILL_PIN      PIN_PB5
#define RED_LED_CTRL    PIN_PC0
#define WHITE_LED_CTRL  PIN_PB2
#define BUTTON_PIN      PIN_PC1

#define DEBOUNCE_TIME     11
#define FUEL_CHECK_TIME   2000            // button press time to trigger batt fuel check
#define MODE_OFF_TIME     4000            // button press time to trigger MODE_OFF
#define MODE_CHANGE_TIME  20              // button press time to trigger mode change
#define EEPROM_ENDURANCE  100000          // Number of cycles EEPROM can be erased/written
#define BATTERY_LVL_MIN   3.0f
#define BATTERY_LVL_MAX   4.2f

#define EEPROM_ID_HIGH_BYTE       (EEPROM.begin())
#define EEPROM_ID_LOW_BYTE        (EEPROM.begin()+1)
#define EEPROM_COUNT_ADDRESS      (EEPROM.begin()+2)
// #define EEPROM_DEVICE_STATE_ADDRESS   (EEPROM.begin()+3)
#define EEPROM_MODE_REF_ADDRESS   (EEPROM.begin()+3)
#define EEPROM_MODE_ADDRESS       (EEPROM.begin()+4)

SFE_MAX1704X lipo(MAX1704X_MAX17048); // Create a MAX17048 object
uint8_t EEPROM_Write_Count;
uint8_t EEPROM_Mode_Addr;
uint16_t DEVICE_id = 0x1616;          // ATTiny1616

typedef enum {      // white LED state
    MODE_OFF = 0,
    MODE_1,
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

uint32_t led1_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{1000, 1000}, {1000,1},      {1000,1},       {1000,1}};
uint32_t led2_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {1000,1000},   {1000,1},       {1000,1}};
uint32_t led3_chg_sequence[LVL_MAX][LED_BLINK_STEPS_CHG]       = {{0,0},        {0,0},         {1000,1000},    {1000,1}};

MODE_e current_mode = MODE_1;
volatile BATT_LVL_e batt_lvl = LVL_20;
volatile uint32_t timerB1_counter=0;
volatile uint64_t button_press_time=0;
volatile BATT_LVL_e chg_sequence = (BATT_LVL_e)5;

volatile bool is_button_pressed = false;
volatile bool is_usb_connected=false;
volatile bool is_turned_on=true;
volatile bool batt_lvl_acquire_flag=false;
volatile bool mode_change_flag=false;
volatile bool state_change_flag=false;
volatile bool device_state_changed=false;

bool is_device_wakeup=true;

void disableUnusedPeripherals()
{
  // Disable the Analog-to-Digital Converter (ADC)
  ADC0.CTRLA = 0;    // Disable ADC by clearing the control register
  ADC1.CTRLA = 0;    // Disable ADC by clearing the control register

  // Disable the Digital-to-Analog Converter (DAC)
  DAC0.CTRLA = 0;    // Disable DAC by clearing the control register
  DAC1.CTRLA = 0;    // Disable DAC by clearing the control register
  DAC2.CTRLA = 0;    // Disable DAC by clearing the control register

  // Disable the Analog Comparator (AC)
  AC0.CTRLA = 0;     // Disable Analog Comparator
  AC1.CTRLA = 0;     // Disable Analog Comparator
  AC2.CTRLA = 0;     // Disable Analog Comparator

  // Disable the Serial Peripheral Interface (SPI)
  SPI0.CTRLA = 0;    // Disable SPI by clearing the control register

  // Disable the Timer/Counter D (TCD) if present
  // TCD0.CTRLA = 0;    // CANNOT DISABLE THIS TIMER

  // Disable the USARTs (USART0 and USART1)
  // USART0.CTRLA = 0;  // Disable USART0

  // Disable Two-Wire Interface (TWI, I2C)
  TWI0.MCTRLA = 0;   // Disable TWI master mode
  TWI0.SCTRLA = 0;   // Disable TWI slave mode

  // Disable the Real-Time Counter (RTC)
  RTC.CTRLA = 0;     // Disable RTC

  // Disable the Event System (EVSYS)
  EVSYS.SYNCCH0 = 0; // Disable all event channels
  EVSYS.ASYNCCH0 = 0;

  // Disable the Watchdog Timer (WDT)
  CCP = CCP_IOREG_gc;     // Configuration Change Protection for writing WDT registers
  WDT.CTRLA = 0;          // Disable the Watchdog Timer

  // Disable Configurable Custom Logic (CCL)
  CCL.CTRLA = 0;          // Disable the CCL

  // Disable the Brown-Out Detector (BOD) unless needed
  CCP = CCP_IOREG_gc;     // Configuration Change Protection for writing BOD registers
  BOD.CTRLA = 0;          // Disable BOD
}

// ISR for main WHITE/RED LEDs
// void TCA0_cmpr2_isr()
// {
//   if(current_mode==MODE_4)
//   {
//     digitalWrite(RED_LED_CTRL, LOW);
//   }
//   // else//(current_mode!=MODE_4)
//   // {
//   //   digitalWrite(WHITE_LED_CTRL, LOW);
//   // }
// }
// void TCA0_overflow_isr()
// {
//   if(current_mode==MODE_4)
//   {
//     digitalWrite(RED_LED_CTRL, HIGH);
//   }
//   // else//(current_mode!=MODE_4)
//   // {
//   //   digitalWrite(WHITE_LED_CTRL, HIGH);
//   // }
// }

void update_led(MODE_e mode)
{
    TCA0_Interface.Stop();
    TCB0_Interface.Stop();
    TCA0_Interface.setDuty(0);
    digitalWrite(RED_LED_CTRL, LOW);
    // digitalWrite(WHITE_LED_CTRL, LOW);
    switch(mode)
    {
        case MODE_4:
            // TCB0_Interface.setDuty(15);
            TCB0_Interface.Start();
            break;
        case MODE_1:
            TCA0_Interface.setDuty(6);
            TCA0_Interface.Start();
            break;
        case MODE_2:
            TCA0_Interface.setDuty(18);
            TCA0_Interface.Start();
            break;
        case MODE_3:
            TCA0_Interface.setDuty(37);
            TCA0_Interface.Start();
            break;
        default:
            TCA0_Interface.Stop();
            TCB0_Interface.Stop();
            TCA0_Interface.setDuty(0);
            // TCB0_Interface.setDuty(0);
            break;
    }
}

// void switch_modes(uint8_t *mode, bool is_activity_change)
// {
//     uint8_t activity_bit = *mode & (1 << 7);
//     uint8_t extracted_mode = *mode & 0x0F;

//     // Rotate the mode bits to the left, and wrap around using modulo 16
//     extracted_mode = (extracted_mode << 1) & 0x0F;
//     if (extracted_mode == 0)
//     {
//         extracted_mode = MODE_1;
//     }

//     // Combine the new mode with the activity bit
//     *mode = extracted_mode | activity_bit;
// }

void update_mode(MODE_e* mode)
{
    if((*mode > MODE_4))
    {
        *mode = MODE_1;
    }

    eeprom_mode_write(*mode);
    update_led(*mode);
}

void led_control_chg(BATT_LVL_e lvl, uint8_t step, uint8_t led)
{
  LED[led].timer = timerB1_counter;
  LED[led].step = step%2;
  LED[led].state = ((step+1) % 2);
  switch(led)
  {
      case 0:
          LED[led].time = led1_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
              status_LED_toggler(led, LED[led].state);
          }
          break;
      case 1:
          LED[led].time = led2_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
              status_LED_toggler(led, LED[led].state);
          }
          break;
      case 2:
          LED[led].time = led3_chg_sequence[lvl][LED[led].step];
          if((step == 0) && (LED[led].time!=0))
          {
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
        TCB1_Interface.PeriodCountSet(0);
        // TCB1_Write(0);
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
        TCB1_Interface.Start();
        // TCB1_Start();
    }
    else
    {
        TCB1_Interface.Stop();
        // TCB1_Stop();
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
            else
            {
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

float mapValue(float *input)      // arduino map function does not allow decimal point values
{
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
BATT_LVL_e batt_soc_percent(float *batt_soc)
{
    float soc = *batt_soc;

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
    while(!lipo.begin())
    {    }
    lipo.quickStart();
    lipo.setThreshold();
    lipo.disableSOCAlert();
    lipo.setVALRTMin(BATTERY_LVL_MIN);
    // lipo.setVALRTMin(BATTERY_LVL_MIN-0.3f);
    lipo.setVALRTMax(BATTERY_LVL_MAX);
}

void eeprom_setup()
{
  // update EEPROM with initial setup
  // i.e. addr 0,1 holds the device id
  //      addr 2 is for number of times EEPROM_Mode_addr have been written
  //      addr 3 holds the address where mode is stored
  
  EEPROM.update(EEPROM_ID_HIGH_BYTE,(uint8_t)((DEVICE_id & 0xFF00) >>8));
  EEPROM.update(EEPROM_ID_LOW_BYTE,(uint8_t)(DEVICE_id & 0x00FF));
  
  for(uint16_t i=EEPROM_COUNT_ADDRESS; i<EEPROM_SIZE; i++)
  {
    EEPROM.update(i,0);
    status_LED_toggler(0, HIGH);
    delay(10);
    status_LED_toggler(0, LOW);
    delay(10);
  }

  EEPROM.update(EEPROM_COUNT_ADDRESS,100);
  EEPROM.update(EEPROM_MODE_REF_ADDRESS,EEPROM_MODE_ADDRESS);
  EEPROM.update(EEPROM_MODE_ADDRESS,MODE_1);

  status_LED_toggler(2, HIGH);
  delay(100);
  status_LED_toggler(2, LOW);
  // delay(100);
}

void eeprom_init()
{
  uint8_t high_byte = EEPROM.read(EEPROM_ID_HIGH_BYTE);
  uint8_t low_byte = EEPROM.read(EEPROM_ID_LOW_BYTE);

  if((((uint16_t)high_byte << 8) | low_byte) != DEVICE_id)
  {
    eeprom_setup();
  }

  EEPROM_Write_Count = EEPROM.read(EEPROM_COUNT_ADDRESS);
  EEPROM_Mode_Addr = EEPROM.read(EEPROM_MODE_REF_ADDRESS);

  // uint8_t eeprom_value = EEPROM.read(EEPROM_Mode_Addr);
  // is_turned_on = (eeprom_value & 0x80) != 0;
  // current_mode = (MODE_e)(eeprom_value & 0x0F);
  current_mode = (MODE_e)(EEPROM.read(EEPROM_Mode_Addr));
  if(current_mode > MODE_4)
  {
    current_mode = MODE_1;
    eeprom_mode_write(current_mode);
    // eeprom_mode_write(current_mode | (is_turned_on ? 0x80 : 0x00));
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
  EEPROM.update(EEPROM_MODE_REF_ADDRESS, EEPROM_Mode_Addr);
  EEPROM.update(EEPROM_Mode_Addr, mode);
  EEPROM.update(EEPROM_COUNT_ADDRESS, EEPROM_Write_Count);
}

void gpio_init()
{
  pinMode(STATUS_LED_75, OUTPUT);
  pinMode(STATUS_LED_50, OUTPUT);
  pinMode(STATUS_LED_25, OUTPUT);
  pinMode(RED_LED_CTRL, OUTPUT);
  pinMode(WHITE_LED_CTRL, OUTPUT);
  pinMode(U_KILL_PIN, OUTPUT);       // IC removed

  pinMode(U_INT_PIN, INPUT);       // IC removed
  pinMode(BUTTON_PIN, INPUT);
  pinMode(GPOUT_PIN, INPUT);
  pinMode(BATT_CHG_PIN, INPUT);

  // attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), BUTTON_pin_int_handler, FALLING);
  // attachInterrupt(digitalPinToInterrupt(BATT_CHG_PIN), BAT_chg_int_handler, FALLING);
  // set_sleep_mode(SLEEP_MODE_STANDBY);

  // Flash the status LEDs twice to indicate that the mcu is running setup
  for(int i=0; i<4; i++)
  {
    bool ledState = (i == 0 || i == 2) ? HIGH : LOW;
    for(int y=0; y<3; y++)
    {
      status_LED_toggler(y, ledState);
    }
    delay(150);
  }

  Wire.pins(SDA, SCL);
  Wire.begin();
}

void timers_init()
{
  // takeOverTCA0();             // This replaces disabling and resettng the timer, required previously.
  TCA0_Interface.Initialize();                 // Main WHITE Led PWM generating timer
  TCB0_Interface.Initialize();                 // Main RED Led PWM denerating timer
  TCB1_Interface.Initialize();                 // 1ms timer for Status LEDs sequence
  TCA0_Interface.PeriodCountSet(1000);         // set PWM freq to 1KHz

  TCA0_Interface.Stop();
  TCB0_Interface.Stop();
  // timer callbacks for PWM generation
  // TCA0_Compare2CallbackRegister(&TCA0_cmpr2_isr);
  // TCA0_OverflowCallbackRegister(&TCA0_overflow_isr);

  // TCB0_CaptureCallbackRegister(&TCB0_handler);
  TCB1_CaptureCallbackRegister(&status_LED_handler);
}

void process_ButtonState()
{
  if(is_device_wakeup)
    return;

  bool state = !digitalRead(BUTTON_PIN) ;
  if(state)   // button pressed
  {
    if (!is_button_pressed)
    {
      button_press_time = millis();
      is_button_pressed = true;
    }
    else
    {
      uint64_t button_pressed_time_val = millis() - button_press_time;

      if((button_pressed_time_val >= FUEL_CHECK_TIME))
      {
          batt_lvl_acquire_flag = true;
      }
      if((button_pressed_time_val >= (MODE_OFF_TIME)) && !state_change_flag)
      {
          state_change_flag = true;
      }
    }
  }
  else  // button released
  {
    if(is_button_pressed)
    {
      is_button_pressed = false;

      uint64_t button_pressed_time_val = millis() - button_press_time;

      if((button_pressed_time_val >= (MODE_CHANGE_TIME)) && (button_pressed_time_val < FUEL_CHECK_TIME))
      {
          mode_change_flag = true;
      }
      if(state_change_flag)
      {
          state_change_flag = false;
          device_state_changed = false;
      }
    }
  }
}
void setup()
{
  disableUnusedPeripherals();
  eeprom_init();

  gpio_init();
  timers_init();

  batt_fuel_ic_init();

  // update_mode(&current_mode);

  led_sequence_action(false);

  if(!digitalRead(BATT_CHG_PIN))
  {
      is_usb_connected = true;
      is_turned_on = false;
  }

  // Enable global interrupts
  // sei();
}

void loop()
{
  delay(DEBOUNCE_TIME);
  process_ButtonState();

    if(state_change_flag && !device_state_changed)
    {
      device_state_changed = true;
      is_turned_on = !is_turned_on;
      if(!is_turned_on)
      {
        update_led(MODE_OFF);
      }
      else
      {
        update_led(current_mode);
      }
    }

    if (is_device_wakeup && (!is_usb_connected & is_turned_on))
    {
      update_mode(&current_mode);
    }

    if(mode_change_flag && is_turned_on)// && !is_usb_connected)            // if button pressed once change mode accordingly
    {
        current_mode = (MODE_e)(current_mode + 1);
        update_mode(&current_mode);
        mode_change_flag = false;
    }
    else
    {
        mode_change_flag = false;
    }

    if(batt_lvl_acquire_flag && !is_usb_connected)       // if button held for 2secs, flash onboard LEDs based on chg lvl
    {
        batt_lvl_acquire_flag = false;
        led_sequence_action(0);
        float current_batt_lvl = lipo.getVoltage();
        batt_lvl = batt_lvl_percent(&current_batt_lvl);
      // batt_lvl = LVL_20;
        led_sequence_action(1);
    }

    if(is_usb_connected)
    {
      // led_sequence_action(0);
      // float current_batt_lvl = lipo.getVoltage();
      float current_batt_lvl = lipo.getSOC();
      // float current_batt_lvl = 3.85f;
      batt_lvl = batt_soc_percent(&current_batt_lvl);
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

    while(is_device_wakeup)
    {
      if(digitalRead(BUTTON_PIN))
      {
        is_device_wakeup = false;
        // update_mode(&current_mode);
      }
    }

  if(!digitalRead(GPOUT_PIN))            // check batt fuel guage alert pin state
  {
    lipo.clearAlert();
    uint8_t status = lipo.getStatus();
    float current_batt_lvl = lipo.getVoltage();
    if(lipo.isVoltageLow() || lipo.isLow())
    {
        digitalWrite(U_KILL_PIN, HIGH);
    }
  }

  if(!digitalRead(BATT_CHG_PIN))            // check if usb is connected, ACTIVE LOW
  {
    if(!is_usb_connected)
    {
      // update_led(MODE_OFF);
      is_usb_connected = true;
    }
  }
  else if(is_usb_connected)
  {
      is_usb_connected = false;
      led_sequence_action(0);
      // if(!is_turned_on)
      // {
      //   digitalWrite(U_KILL_PIN, HIGH);
      // }
      is_device_wakeup = true;
  }
}
