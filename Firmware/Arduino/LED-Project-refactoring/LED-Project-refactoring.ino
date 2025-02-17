#include "definitions.h"
#include "Libraries/Button/button.hpp"
// #include "Libraries/Device/device.hpp"
#include "Libraries/LED_Manager/led_manager.hpp"
// #include "Libraries/Output_LED/output_led.hpp"
#include "Libraries/Batt_Manager/batt_Manager.hpp"
// #include "Libraries/Timer/timer.hpp"

// Button          button(BUTTON_PIN, DEBOUNCE_TIME);
// OutputLed       outputLed(RED_LED_CTRL_PIN, WHITE_LED_CTRL_PIN);
StatusLeds      leds[STATUS_LEDS] = {STATUS_LED_25_PIN, STATUS_LED_50_PIN, STATUS_LED_75_PIN};
// SFE_MAX1704X    battMonitor(MAX1704X_MAX17048);
// BattManager     battManager(&battMonitor, BATT_CHG_PIN, GPOUT_PIN, SDA_PIN, SCL_PIN);
StatusManager*  statusLedController;
// Timer*          timer;
// Device*         device;

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
  USART0.CTRLA = 0;  // Disable USART0

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

void setup()
{
  disableUnusedPeripherals();
  
  statusLedController = StatusManager::getInstance();
  // battManager = new BattManager(&battMonitor, BATT_CHG_PIN, GPOUT_PIN, SDA_PIN, SCL_PIN);
  // outputLedController = new OutputLed(RED_LED_CTRL_PIN, WHITE_LED_CTRL_PIN);
  // timer = new Timer(statusLedController);
  // device = new Device(outputLedController, statusLedController, leds[0], leds[1], leds[2]);
 
  // button.subscribe();
  // battManager.subscribe();
  
  for(int i=0; i<STATUS_LEDS; i++)
  {
    statusLedController->addLed(&leds[i]);
  }
  // battManager.init();


  // // eeprom_init();

  // gpio_init();
  // timers_init();

  // batt_fuel_ic_init();

  // // update_mode(&current_mode);

  // led_sequence_action(0);

  // // Enable global interrupts
  // // sei();

  statusLedController->managerTest(DISCHARGING, LVL_20);

}

void loop()
{
  // button.update();
  // battManager.update();

  // for(int i=0; i<STATUS_LEDS; i++)
  // {
  //   statusLedController->ledOn(i);
  // }
  // delay(3000);
  // for(int i=0; i<STATUS_LEDS; i++)
  // {
  //   statusLedController->ledOff(i);
  // }
  // delay(3000);
  statusLedController->statusLedHandlerWrapper();
}
