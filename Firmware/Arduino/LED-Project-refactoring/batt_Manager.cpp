#include "Libraries/Batt_Manager/batt_Manager.hpp"
#include <Wire.h>

BattManager* BattManager::instance = NULL;

float mapValue(float& input)      // arduino map function does not allow decimal point values
{
    return ((input - BATTERY_LVL_MIN) * (100.0 - 0.0)) / (BATTERY_LVL_MAX - BATTERY_LVL_MIN);
}

void BattManager::init()
{
    Wire.pins(_sda_pin, _scl_pin);
    Wire.begin();
    
    while(!_batt->begin())
    {
        _batt->wake();
        _batt->enableComparator();
    }
    if(_batt->isReset(true))
    {
        _batt->quickStart();
    }
    _batt->setResetVoltage(2.5f);
    // _batt->disableSOCAlert();
    // _batt->disableAlert();
    _batt->setThreshold(LOW_SOC_THRESHOLD);
    _batt->setVALRTMin(BATTERY_LVL_MIN-0.2f);
    _batt->setVALRTMax(BATTERY_LVL_MAX);

    attachInterrupt(digitalPinToInterrupt(_chg_pin), BattManager::battChgInterruptWrapper, CHANGE);
    attachInterrupt(digitalPinToInterrupt(_max17048_alrt_pin), BattManager::fuelICInterruptWrapper, FALLING);

    updateBattStatus();
}

void BattManager::update()
{
    if(_is_usb_connected && !_is_charging)
    {
        if((BATTERY_LVL_MAX-0.15f) > (_batt->getVoltage()))
        {
            _is_usb_connected = false;
            notify(DEVICE_STATUS_UPDATE, (uint8_t)DISCHARGING);
        }
    }
}

void BattManager::updateBattStatus()
{
    _batt_soc = _batt->getSOC();
    _batt_voltage = _batt->getVoltage();
    BATT_LVL_e temp = batt_lvl_percent(_batt_soc);
    // BATT_LVL_e temp = batt_lvl_percent(_batt_voltage);
    if(temp != _batt_level)
    {
        _batt_level = temp;
        notify(BATTERY_UPDATE_EVENT, (uint8_t)_batt_level);
    }
}

BATT_LVL_e BattManager::getBattLvl()
{
    return _batt_level;
}

float BattManager::getBattVoltage()
{
    return _batt_voltage;
}

int BattManager::getBattSOC()
{
    return _batt_soc;
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
    else if(soc<95.0)
    {
        return LVL_100;
    }
    return LVL_MAX;
}
BATT_LVL_e BattManager::batt_lvl_percent(int& soc)
{
    // float soc = *batt_soc;

    if(soc<=20)
    {
        return LVL_20;
    }
    else if(soc<=33)
    {
        return LVL_33;
    }
    else if(soc<=66)
    {
        return LVL_66;
    }
    else if(soc<95)
    {
        return LVL_100;
    }
    return LVL_MAX;
}

void BattManager::notify(EVENTS_LIST_e eventID, uint8_t val)
{
    if(NULL != _observer)
    {
        _observer->registerEvent(eventID, val);
    }
}

void BattManager::chgInterruptHandler()
{
    bool state = !digitalRead(_chg_pin);        // Active low
    if(state)
    {
        _is_charging = true;
        _is_usb_connected = true;
        notify(DEVICE_STATUS_UPDATE, (uint8_t)CHARGING);
    }
    else
    {
        _is_charging = false;
    }
}

void BattManager::alrtInterruptHandler()
{
    _batt->clearAlert();          // clear the ALRT pin
    if(_batt->isChange(true))
    {
        updateBattStatus();
    }
    if(_batt->isLow(true) || _batt->isVoltageLow(true))
    {

    }
}

void BattManager::battChgInterruptWrapper()
{
    if(instance)
    {
        instance->chgInterruptHandler();
    }
}

void BattManager::fuelICInterruptWrapper()
{
    if(instance)
    {
        instance->alrtInterruptHandler();
    }
}
