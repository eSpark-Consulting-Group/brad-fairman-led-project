#include "Libraries/Batt_Manager/batt_Manager.hpp"
#include <Wire.h>

float mapValue(float& input)      // arduino map function does not allow decimal point values
{
    return ((input - BATTERY_LVL_MIN) * (100.0 - 0.0)) / (BATTERY_LVL_MAX - BATTERY_LVL_MIN);
}

void BattManager::init()
{
    Wire.pins(_sda_pin, _scl_pin);
    Wire.begin();
    
    while(!_batt->begin())
    {    }
    if(_batt->isReset(true))
    {
        _batt->quickStart();
    }
    // _batt->disableSOCAlert();
    _batt->setThreshold(LOW_SOC_THRESHOLD);
    _batt->setVALRTMin(BATTERY_LVL_MIN-0.1f);
    _batt->setVALRTMax(BATTERY_LVL_MAX);

    update();
}

void BattManager::update()
{
    _batt_soc = _batt->getSOC();
    _batt_voltage = _batt->getVoltage();
}

BATT_LVL_e BattManager::getBattLvl()
{
    // return batt_lvl_percent(_batt_voltage);
    return batt_soc_percent(_batt_soc);
}

float BattManager::getBattVoltage()
{
    return _batt_voltage;
}

float BattManager::getBattSOC()
{
    return _batt_soc;
}

void BattManager::battInterruptHandler()
{

}

void BattManager::alrtInterruptHandler()
{
    _batt->clearAlert()          // clear the ALRT pin
}

BATT_LVL_e BattManager::batt_lvl_percent(float& batt_voltage)
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
BATT_LVL_e BattManager::batt_soc_percent(float& soc)
{
    // float soc = *batt_soc;

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

void BattManager::notify(uint8_t eventID, uint8_t val)
{
    if(NULL != _observer)
        _observer->registerEvent(eventID, (uint8_t)val);
}
