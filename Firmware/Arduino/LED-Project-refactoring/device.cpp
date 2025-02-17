#include "Libraries/Device/device.hpp"


void Device::setup()
{
    eepromInit();
    _output_led_controller->setModeVar(&_current_mode);
    _status_led_controller->addBattFlag(&_batt_lvl);
}

void Device::eepromModeWrite(MODE_e mode)
{
    _eeprom_write_count++;
    if(_eeprom_write_count > (EEPROM_ENDURANCE-100))
    {
        _eeprom_write_count=1;
        _eeprom_mode_addr++;
    }
    EEPROM.update(EEPROM_MODE_REF_ADDRESS, _eeprom_mode_addr);
    EEPROM.update(_eeprom_mode_addr, mode);
    EEPROM.update(EEPROM_COUNT_ADDRESS, _eeprom_write_count);
}

void Device::eepromSetup()
{
    // update EEPROM with initial setup
    // i.e. addr 0,1 holds the device id
    //      addr 2 is for number of times EEPROM_Mode_addr have been written
    //      addr 3 holds the address where mode is stored

    EEPROM.update(EEPROM_ID_HIGH_BYTE,(uint8_t)((_device_id & 0xFF00) >>8));
    EEPROM.update(EEPROM_ID_LOW_BYTE,(uint8_t)(_device_id & 0x00FF));

    for(uint16_t i=EEPROM_COUNT_ADDRESS; i<EEPROM_SIZE; i++)
    {
        EEPROM.update(i,0xFF);
        _status_led_controller->ledOn(1);
        delay(10);
        _status_led_controller->ledOff(1);
        delay(10);
    }

    EEPROM.update(EEPROM_COUNT_ADDRESS,100);
    EEPROM.update(EEPROM_MODE_REF_ADDRESS,EEPROM_MODE_ADDRESS);
    EEPROM.update(EEPROM_MODE_ADDRESS,MODE_1);

    // status_LED_toggler(2, HIGH);
    _status_led_controller->ledOn(3);
    delay(100);
    // status_LED_toggler(2, LOW);
    _status_led_controller->ledOff(3);
}

void Device::eepromInit()
{
    uint8_t high_byte = EEPROM.read(EEPROM_ID_HIGH_BYTE);
    uint8_t low_byte = EEPROM.read(EEPROM_ID_LOW_BYTE);

    if(((((uint16_t)high_byte) << 8) | low_byte) != _device_id)     // if device is fresh or not setup properly, initialize eeprom
    {
        eepromSetup();
    }

    _eeprom_write_count = EEPROM.read(EEPROM_COUNT_ADDRESS);
    // EEPROM_Address = EEPROM_MODE_REF_ADDRESS;
    _eeprom_mode_addr = EEPROM.read(EEPROM_MODE_REF_ADDRESS);

    _current_mode = (MODE_e)(EEPROM.read(_eeprom_mode_addr));
    if(_current_mode > MODE_4)
    {
        _current_mode = MODE_1;
        eepromModeWrite(_current_mode);
    }
}


//----------------------------------------------------------------------


