#include "batt_Manager.hpp"
#include <Wire.h>

void BattManager::setup()
{
    Wire.pins(_sda_pin, _scl_pin);
    Wire.begin();
    
    while(!_batt.begin())
    {    }
    _batt.quickStart();
    _batt.setThreshold(4);
    _batt.disableSOCAlert();
    _batt.setVALRTMin(BATTERY_LVL_MIN);
    _batt.setVALRTMax(BATTERY_LVL_MAX);
}

static void BattManager::usbInterruptHandler()
{

}

static void BattManager::maxInterruptHandler()
{
    
}
