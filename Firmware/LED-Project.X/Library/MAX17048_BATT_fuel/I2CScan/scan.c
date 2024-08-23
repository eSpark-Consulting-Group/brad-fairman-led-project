#include "mcc_generated_files/system/system.h"

void i2cScan()
{
    i2c_host_error_t error;
    for(address = 1; address < 127; address++ ) 
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        I2C0_Host_Write(address, 1, 1);
        error = I2C0_Host_ErrorGet();

        if (error == I2C_ERROR_NONE)
        {
            nDevices++;
        }
        else if (error==4) 
        {
            // Serial.print("Unknow error at address 0x");
            if (address<16) 
            // Serial.print("0");
            // Serial.println(address,HEX);
        }    
    }
}