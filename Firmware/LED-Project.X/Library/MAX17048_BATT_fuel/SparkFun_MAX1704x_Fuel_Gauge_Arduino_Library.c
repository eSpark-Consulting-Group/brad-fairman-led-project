#include "SparkFun_MAX1704x_Fuel_Gauge_Arduino_Library.h"

int _device = MAX1704X_MAX17048; // Default to MAX17043
float _full_scale = 5.12; // Default: full-scale for the MAX1704
  
static long constrain(long x, long a, long b) {
  if (x < a) {
    return a;
  } else if (x > b) {
    return b;
  } else {
    return x;
  }
}

void SFE_MAX1704X_Init(sfe_max1704x_devices_e device)
{
  // Constructor
  SFE_MAX1704X_setDevice(device);
}

// Change the device type if required. Do this after instantiation but before .begin
void SFE_MAX1704X_setDevice(sfe_max1704x_devices_e device)
{
  // Record the device type
  _device = device;

  // Define the full-scale voltage for VCELL based on the device
  switch (device)
  {
    case MAX1704X_MAX17044:
      _full_scale = 10.24; // MAX17044 VCELL is 12-bit, 2.50mV per LSB
      break;
    case MAX1704X_MAX17048:
      _full_scale = 5.12; // MAX17048 VCELL is 16-bit, 78.125uV/cell per LSB
      break;
    case MAX1704X_MAX17049:
      _full_scale = 10.24; // MAX17049 VCELL is 16-bit, 78.125uV/cell per LSB (i.e. 156.25uV per LSB)
      break;
    default: // Default is the MAX17043
      _full_scale = 5.12; // MAX17043 VCELL is 12-bit, 1.25mV per LSB
      break;
  }
}

bool SFE_MAX1704X_begin()
{
  if (SFE_MAX1704X_isConnected() == false)
  {
    return (false);
  }

  return (true);
}

//Returns true if device is present
bool SFE_MAX1704X_isConnected(void)
{
  //Updated to resolve issue #4 Dec 27th 2021
  //Also avoid using the standard "if device answers on _deviceAddress" test
  //(https://github.com/sparkfun/Arduino_Apollo3/issues/400#issuecomment-992631994)
  bool success = false;
  uint8_t retries = 3;
  uint16_t version = 0;

  while ((success == false) && (retries > 0))
  {
    // I2C0_Open(MAX1704x_ADDRESS);
    uint8_t reg_addr = MAX17043_VERSION;
    uint8_t read_data[2] = {0};
    // I2C0_SetBuffer(&reg_addr,1);
    // I2C0_MasterWrite(MAX17043_VERSION); // Attempt to read the version register

    // _i2cPort->beginTransmission(MAX1704x_ADDRESS);
    // _i2cPort->write(MAX17043_VERSION); // Attempt to read the version register
    // _i2cPort->endTransmission(false); // Don't release the bus

    if (TWI0_Write(MAX1704x_ADDRESS, &reg_addr, 1)) // Attempt to read the version (2 bytes)
    // if (_i2cPort->requestFrom(MAX1704x_ADDRESS, 2) == 2) // Attempt to read the version (2 bytes)
    {
      // uint8_t msb;
      // TWI0_Read(MAX1704x_ADDRESS, &msb, 1);
      // // I2C0_MasterRead();

      // uint8_t lsb;
      // TWI0_Read(MAX1704x_ADDRESS, &lsb, 1);
      // // I2C0_MasterRead();
        i2c_host_error_t error = TWI0_ErrorGet();
        while(TWI0_IsBusy());
       TWI0_Read(MAX1704x_ADDRESS, read_data, 2);
        while(TWI0_IsBusy());
      version = ((uint16_t)read_data[0] << 8) | read_data[1];
      success = true;
    }
    else
    {
      retries--;
      // delay(50);
      for(int i = 0; i<100; i++){
         __asm__("nop"); // Do nothing, just wait
      }
    }
    // I2C0_Close(); // Don't release the bus
  }

  if (!success) // Return now if the version could not be read
  {
    return (success);
  }

  //Extra test - but only for MAX17048/9 - see issue #4
  if (_device >= MAX1704X_MAX17048)
  {
    //Get version should return 0x001_
    //Not a great test but something
    //Supported on 48/49
    if ((version & (1 << 4)) == 0)
      success = false;
  }

  return (success);
}

uint8_t SFE_MAX1704X_quickStart()
{
  // A quick-start allows the MAX17043 to restart fuel-gauge calculations in the
  // same manner as initial power-up of the IC. If an application’s power-up
  // sequence is exceedingly noisy such that excess error is introduced into the
  // IC’s “first guess” of SOC, the host can issue a quick-start to reduce the
  // error. A quick-start is initiated by a rising edge on the QSTRT pin, or
  // through software by writing 4000h to MODE register.

  // Note: on the MAX17048/49 this will also clear / disable EnSleep

  return SFE_MAX1704X_write16(MAX17043_MODE_QUICKSTART, MAX17043_MODE);
}

float SFE_MAX1704X_getVoltage()
{
  uint16_t vCell;
  vCell = SFE_MAX1704X_read16(MAX17043_VCELL);

  if (_device <= MAX1704X_MAX17044)
  {
    // On the MAX17043/44: vCell is a 12-bit register where each bit represents:
    // 1.25mV on the MAX17043
    // 2.5mV on the MAX17044
    vCell = (vCell) >> 4; // Align the 12 bits

    float divider = 4096.0 / _full_scale;

    return (((float)vCell) / divider);
  }
  else
  {
    // On the MAX17048/49: vCell is a 16-bit register where each bit represents 78.125uV/cell per LSB
    // i.e. 78.125uV per LSB on the MAX17048
    // i.e. 156.25uV per LSB on the MAX17049

    float divider = 65536.0 / _full_scale;

    return (((float)vCell) / divider);
  }
}

float SFE_MAX1704X_getSOC()
{
  uint16_t soc;
  float percent;
  soc = SFE_MAX1704X_read16(MAX17043_SOC);
  percent = (float)((soc & 0xFF00) >> 8);
  percent += ((float)(soc & 0x00FF)) / 256.0;

  return percent;
}

uint16_t SFE_MAX1704X_getVersion()
{
  return SFE_MAX1704X_read16(MAX17043_VERSION);
}

//Supported on MAX17048/49
uint8_t SFE_MAX1704X_getID()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t vresetID = SFE_MAX1704X_read16(MAX17048_VRESET_ID);
  return (vresetID & 0xFF);
}

//Default is 0x4B = 75 (7bit, shifted from 0x96__)
//40mV per bit. So default is 3.0V.
uint8_t SFE_MAX1704X_setResetVoltage(uint8_t threshold)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t vreset = SFE_MAX1704X_read16(MAX17048_VRESET_ID);
  vreset &= 0x01FF;                     // Mask out bits to set
  vreset |= ((uint16_t)threshold << 9); // Add new threshold

  return SFE_MAX1704X_write16(vreset, MAX17048_VRESET_ID);
}
uint8_t SFE_MAX1704X_setResetVoltage_f(float threshold)
{
  // 7 bits. LSb = 40mV
  uint8_t thresh = (uint8_t)(constrain(threshold, 0.0, 5.08) / 0.04);
  return SFE_MAX1704X_setResetVoltage(thresh);
}

uint8_t SFE_MAX1704X_getResetVoltage(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t threshold = SFE_MAX1704X_read16(MAX17048_VRESET_ID) >> 9;
  return ((uint8_t)threshold);
}

uint8_t SFE_MAX1704X_enableComparator(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t vresetReg = SFE_MAX1704X_read16(MAX17048_VRESET_ID);
  vresetReg &= ~(1 << 8); //Clear bit to enable comparator
  return SFE_MAX1704X_write16(vresetReg, MAX17048_VRESET_ID);
}

uint8_t SFE_MAX1704X_disableComparator(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t vresetReg = SFE_MAX1704X_read16(MAX17048_VRESET_ID);
  vresetReg |= (1 << 8); //Set bit to disable comparator
  return SFE_MAX1704X_write16(vresetReg, MAX17048_VRESET_ID);
}

float SFE_MAX1704X_getChangeRate(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0.0);
  }

  int16_t changeRate = SFE_MAX1704X_read16(MAX17048_CRATE);
  float changerate_f = changeRate * 0.208;
  return (changerate_f);
}

uint8_t SFE_MAX1704X_getStatus(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint8_t statusReg = SFE_MAX1704X_read16(MAX17048_STATUS) >> 8;
  return (statusReg & 0x7F); //Highest bit is don't care
}

bool SFE_MAX1704X_isReset(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_RI) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_RI << 8);
  }
  return (flag);
}
bool SFE_MAX1704X_isVoltageHigh(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_VH) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_VH << 8);
  }
  return (flag);
}
bool SFE_MAX1704X_isVoltageLow(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_VL) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_VL << 8);
  }
  return (flag);
}
bool SFE_MAX1704X_isVoltageReset(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_VR) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_VR << 8);
  }
  return (flag);
}
bool SFE_MAX1704X_isLow(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_HD) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_HD << 8);
  }
  return (flag);
}
bool SFE_MAX1704X_isChange(bool clear)
{
  uint8_t status = SFE_MAX1704X_getStatus();
  bool flag = (status & MAX1704x_STATUS_SC) > 0;
  if (flag && clear) // Clear the flag if requested
  {
    // Clear the aligned bit in the status register
    SFE_MAX1704X_clearStatusRegBits(MAX1704x_STATUS_SC << 8);
  }
  return (flag);
}

// Clear the specified bit in the MAX17048/49 status register (PRIVATE)
// This requires the bits in mask to be correctly aligned.
// MAX1704x_STATUS_RI etc. will need to be shifted left by 8 bits to become aligned.
uint8_t SFE_MAX1704X_clearStatusRegBits(uint16_t mask)
{
  uint16_t statusReg = SFE_MAX1704X_read16(MAX17048_STATUS);
  statusReg &= ~mask; // Clear the specified bits
  return (SFE_MAX1704X_write16(statusReg, MAX17048_STATUS)); // Write the contents back again
}

uint8_t SFE_MAX1704X_clearAlert()
{
  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  configReg &= ~MAX17043_CONFIG_ALERT; // Clear ALRT bit manually.

  return SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);
}

// getAlert([clear]) - Check if the MAX1704X's ALRT alert interrupt has been
// triggered.
// INPUT: [clear] - If [clear] is true, the alert flag will be cleared if it
// was set.
// OUTPUT: Returns 1 if interrupt is/was triggered, 0 if not.
uint8_t SFE_MAX1704X_getAlert(bool clear)
{
  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  if (configReg & MAX17043_CONFIG_ALERT)
  {
    if (clear) // If the clear flag is set
    {
      configReg &= ~MAX17043_CONFIG_ALERT; // Clear ALRT bit manually.
      SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);
    }
    return 1;
  }

  return 0;
}

// enableSOCAlert() - (MAX17048/49) Enable the SOC change alert
// Returns true if the SOC change alert was enabled successfully
bool SFE_MAX1704X_enableSOCAlert()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (false);
  }

  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  configReg |= MAX17043_CONFIG_ALSC; // Set the ALSC bit
  // Update the config register, return false if the write fails
  if (SFE_MAX1704X_write16(configReg, MAX17043_CONFIG) > 0)
    return (false);
  // Re-Read the config reg
  configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  // Return true if the ALSC bit is set, otherwise return false
  return ((configReg & MAX17043_CONFIG_ALSC) > 0);
}

// disableSOCAlert() - (MAX17048/49) Disable the SOC change alert
// Returns true if the SOC change alert was disabled successfully
bool SFE_MAX1704X_disableSOCAlert()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (false);
  }

  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  configReg &= ~MAX17043_CONFIG_ALSC; // Clear the ALSC bit
  // Update the config register, return false if the write fails
  if (SFE_MAX1704X_write16(configReg, MAX17043_CONFIG) > 0)
    return (false);
  // Re-Read the config reg
  configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  // Return true if the ALSC bit is clear, otherwise return false
  return ((configReg & MAX17043_CONFIG_ALSC) == 0);
}

// Enable or Disable MAX17048 VRESET Alert:
//  EnVr (enable voltage reset alert) when set to 1 asserts
//  the ALRT pin when a voltage-reset event occurs under
//  the conditions described by the VRESET/ ID register.
uint8_t SFE_MAX1704X_enableAlert(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t statusReg = SFE_MAX1704X_read16(MAX17048_STATUS);
  statusReg |= MAX1704x_STATUS_EnVR; // Set EnVR bit
  return SFE_MAX1704X_write16(statusReg, MAX17048_STATUS);
}

uint8_t SFE_MAX1704X_disableAlert(void)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t statusReg = SFE_MAX1704X_read16(MAX17048_STATUS);
  statusReg &= ~MAX1704x_STATUS_EnVR; // Clear EnVR bit
  return SFE_MAX1704X_write16(statusReg, MAX17048_STATUS);
}

uint8_t SFE_MAX1704X_getThreshold()
{
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  uint8_t threshold = (configReg & 0x001F);

  // It has an LSb weight of 1%, and can be programmed from 1% to 32%.
  // The value is (32 - ATHD)%, e.g.: 00000=32%, 00001=31%, 11111=1%.
  // Let's convert our percent to that first:
  threshold = 32 - threshold;
  return threshold;
}

uint8_t SFE_MAX1704X_setThreshold(uint8_t percent)
{
  // The alert threshold is a 5-bit value that sets the state of charge level
  // where an interrupt is generated on the ALRT pin.

  // It has an LSb weight of 1%, and can be programmed from 1% to 32%.
  // The value is (32 - ATHD)%, e.g.: 00000=32%, 00001=31%, 11111=1%.
  // Let's convert our percent to that first:
  percent = (uint8_t)constrain((float)percent, 0.0, 32.0);
  percent = 32 - percent;

  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  configReg &= 0xFFE0;  // Mask out threshold bits
  configReg |= percent; // Add new threshold

  return SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);
}

// In sleep mode, the IC halts all operations, reducing current
// consumption to below 1μA. After exiting sleep mode,
// the IC continues normal operation. In sleep mode, the
// IC does not detect self-discharge. If the battery changes
// state while the IC sleeps, the IC cannot detect it, causing
// SOC error. Wake up the IC before charging or discharging.
uint8_t SFE_MAX1704X_sleep()
{
  if (_device > MAX1704X_MAX17044)
  {
    // On the MAX17048, we also have to set the EnSleep bit in the MODE register
    uint8_t result =  SFE_MAX1704X_write16(MAX17048_MODE_ENSLEEP, MAX17043_MODE);
    if (result)
      return (result); // Write failed. Bail.
  }

  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  if (configReg & MAX17043_CONFIG_SLEEP)
  {
    return MAX17043_GENERIC_ERROR; // Already sleeping, do nothing but return an error
  }

  configReg |= MAX17043_CONFIG_SLEEP; // Set sleep bit

  return SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);
}

uint8_t SFE_MAX1704X_wake()
{
  // Read config reg, so we don't modify any other values:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  if (!(configReg & MAX17043_CONFIG_SLEEP))
  {
    return MAX17043_GENERIC_ERROR; // Already sleeping, do nothing but return an error
  }
  configReg &= ~MAX17043_CONFIG_SLEEP; // Clear sleep bit

  uint8_t result = SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);

  if (result)
    return (result); // Write failed. Bail.

  if (_device > MAX1704X_MAX17044)
  {
    // On the MAX17048, we should also clear the EnSleep bit in the MODE register
    // Strictly, this will clear the QuickStart bit too. Which is probably a good thing,
    // as I don't think we can do a read-modify-write?
    return SFE_MAX1704X_write16(0x0000, MAX17043_MODE);
  }
  else
  {
    return (result);
  }
}

// Writing a value of 0x5400 to the CMD Register causes
// the device to completely reset as if power had been
// removed (see the Power-On Reset (POR) section). The
// reset occurs when the last bit has been clocked in. The
// IC does not respond with an I2C ACK after this command
// sequence.
// Output: Positive integer on success, 0 on fail.
uint8_t SFE_MAX1704X_reset()
{
  return SFE_MAX1704X_write16(MAX17043_COMMAND_POR, MAX17043_COMMAND);
}

uint8_t SFE_MAX1704X_getCompensation()
{
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  uint8_t compensation = (configReg & 0xFF00) >> 8;
  return compensation;
}

uint16_t SFE_MAX1704X_getConfigRegister()
{
  return SFE_MAX1704X_read16(MAX17043_CONFIG);
}

uint8_t SFE_MAX1704X_setCompensation(uint8_t newCompensation)
{
  // The CONFIG register compensates the ModelGauge algorith. The upper 8 bits
  // of the 16-bit register control the compensation.
  // Read the original configReg, so we can leave the lower 8 bits alone:
  uint16_t configReg = SFE_MAX1704X_read16(MAX17043_CONFIG);
  configReg &= 0x00FF; // Mask out compensation bits
  configReg |= ((uint16_t)newCompensation) << 8;
  return SFE_MAX1704X_write16(configReg, MAX17043_CONFIG);
}

// VALRT Register:
//  This register is divided into two thresholds: Voltage alert
//  maximum (VALRT.MAX) and minimum (VALRT. MIN).
//  Both registers have 1 LSb = 20mV. The IC alerts while
//  VCELL > VALRT.MAX or VCELL < VALRT.MIN
uint8_t SFE_MAX1704X_setVALRTMax(uint8_t threshold)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t valrt = SFE_MAX1704X_read16(MAX17048_CVALRT);
  valrt &= 0xFF00; // Mask off max bits
  valrt |= (uint16_t)threshold;
  return SFE_MAX1704X_write16(valrt, MAX17048_CVALRT);
}
uint8_t SFE_MAX1704X_setVALRTMax_f(float threshold)
{
  uint8_t thresh = (uint8_t)(constrain(threshold, 0.0, 5.1) / 0.02);
  return SFE_MAX1704X_setVALRTMax(thresh);
}

uint8_t SFE_MAX1704X_getVALRTMax()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t valrt = SFE_MAX1704X_read16(MAX17048_CVALRT);
  valrt &= 0x00FF; // Mask off max bits
  return ((uint8_t)valrt);
}

uint8_t SFE_MAX1704X_setVALRTMin(uint8_t threshold)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t valrt = SFE_MAX1704X_read16(MAX17048_CVALRT);
  valrt &= 0x00FF; // Mask off min bits
  valrt |= ((uint16_t)threshold) << 8;
  return SFE_MAX1704X_write16(valrt, MAX17048_CVALRT);
}
uint8_t SFE_MAX1704X_setVALRTMin_f(float threshold)
{
  uint8_t thresh = (uint8_t)(constrain(threshold, 0.0, 5.1) / 0.02);
  return SFE_MAX1704X_setVALRTMin(thresh);
}

uint8_t SFE_MAX1704X_getVALRTMin()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t valrt = SFE_MAX1704X_read16(MAX17048_CVALRT);
  valrt >>= 8; // Shift min into LSB
  return ((uint8_t)valrt);
}

bool SFE_MAX1704X_isHibernating()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (false);
  }

  uint16_t mode = SFE_MAX1704X_read16(MAX17043_MODE);
  return ((mode & MAX17048_MODE_HIBSTAT) > 0);
}

uint8_t SFE_MAX1704X_getHIBRTActThr()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t hibrt = SFE_MAX1704X_read16(MAX17048_HIBRT);
  hibrt &= 0x00FF; // Mask off Act bits
  return ((uint8_t)hibrt);
}

uint8_t SFE_MAX1704X_setHIBRTActThr(uint8_t threshold)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t hibrt = SFE_MAX1704X_read16(MAX17048_HIBRT);
  hibrt &= 0xFF00; // Mask off Act bits
  hibrt |= (uint16_t)threshold;
  return SFE_MAX1704X_write16(hibrt, MAX17048_HIBRT);
}
uint8_t SFE_MAX1704X_setHIBRTActThr_f(float threshold)
{
  // LSb = 1.25mV
  uint8_t thresh = (uint8_t)(constrain(threshold, 0.0, 0.31875) / 0.00125);
  return SFE_MAX1704X_setHIBRTActThr(thresh);
}

uint8_t SFE_MAX1704X_getHIBRTHibThr()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (0);
  }

  uint16_t hibrt = SFE_MAX1704X_read16(MAX17048_HIBRT);
  hibrt >>= 8; // Shift HibThr into LSB
  return ((uint8_t)hibrt);
}

uint8_t SFE_MAX1704X_setHIBRTHibThr(uint8_t threshold)
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  uint16_t hibrt = SFE_MAX1704X_read16(MAX17048_HIBRT);
  hibrt &= 0x00FF; // Mask off Hib bits
  hibrt |= ((uint16_t)threshold) << 8;
  return SFE_MAX1704X_write16(hibrt, MAX17048_HIBRT);
}
uint8_t SFE_MAX1704X_setHIBRTHibThr_f(float threshold)
{
  // LSb = 0.208%/hr
  uint8_t thresh = (uint8_t)(constrain(threshold, 0.0, 53.04) / 0.208);
  return SFE_MAX1704X_setHIBRTHibThr(thresh);
}

uint8_t SFE_MAX1704X_enableHibernate()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  return SFE_MAX1704X_write16(MAX17048_HIBRT_ENHIB, MAX17048_HIBRT);
}

uint8_t SFE_MAX1704X_disableHibernate()
{
  if (_device <= MAX1704X_MAX17044)
  {
    return (MAX17043_GENERIC_ERROR);
  }

  return SFE_MAX1704X_write16(MAX17048_HIBRT_DISHIB, MAX17048_HIBRT);
}

uint8_t SFE_MAX1704X_write16(uint16_t data, uint8_t address)
{
  // uint8_t msb, lsb;
  // msb = (data & 0xFF00) >> 8;
  // lsb = (data & 0x00FF);

  // uint8_t* ptr = (uint8_t*)&data;
  // uint8_t write_data[2] = {ptr[1], ptr[0]}; 
  data = ((data & 0xFF00) >> 8 ) || ((data & 0x00FF) << 8 );
  while(TWI0_IsBusy);
  TWI0_Write(MAX1704x_ADDRESS, (uint8_t*)&data, 2);
  while(TWI0_IsBusy);
  return true;
  // _i2cPort->beginTransmission(MAX1704x_ADDRESS);
  // _i2cPort->write(address);
  // _i2cPort->write(msb);
  // _i2cPort->write(lsb);
  // return (_i2cPort->endTransmission());
}

uint16_t SFE_MAX1704X_read16(uint8_t address)
{
  bool success = false;
  uint8_t retries = 3;
  uint16_t result = 0;

  while ((success == false) && (retries > 0))
  {
    uint8_t read_data[2];
    // I2C0_Open(MAX1704x_ADDRESS);
    // uint8_t reg_addr = address;
    // I2C0_SetBuffer(&reg_addr,1);

    // _i2cPort->beginTransmission(MAX1704x_ADDRESS);
    // _i2cPort->write(address);
    // _i2cPort->endTransmission(false); // Don't release the bus

    if (TWI0_WriteRead(MAX1704x_ADDRESS, &address, 1, read_data, 2))
    {
      // uint8_t msb;
      // I2C0_SetBuffer(&msb, 1);
      // I2C0_MasterRead();

      // uint8_t lsb;
      // I2C0_SetBuffer(&lsb, 1);
      // I2C0_MasterRead();
        while(TWI0_IsBusy);
      result = ((uint16_t)read_data[0] << 8) | read_data[1];
      success = true;
    }
    else
    {
      retries--;
      // delay(50);
      for(int i = 0; i<100; i++){
         __asm__("nop"); // Do nothing, just wait
      }
    }
    // I2C0_Close(); // Don't release the bus
  }

  return (result);
}
