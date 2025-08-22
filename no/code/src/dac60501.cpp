#include "dac60501.h"

DAC60501::DAC60501(I2C& i2c_dev): i2c(i2c_dev) {}


bool DAC60501::init(int8_t dacId)
{
    deviceId = dacId;
    
    /* I2C TEST ERSATZ*/
    i2c.start();
    uint8_t result = i2c.writeAddress(deviceId, RD);
    i2c.stop();

    if(result == ACK)
    {
        return true;
    }

    LOGWARN("NO ACK FROM ADDR 0x%X", deviceId);
    return false;
}

bool DAC60501::setOutputVoltagerange(enum MAXVOUT vMax)
{
    uint16_t gainReg = i2c.readRegister16(deviceId, DAC60501_REG_GAIN);
    currentMaxVoltage = vMax;
    switch(vMax)
    {
        case MAX_1V25:
            i2c.writeRegister16(deviceId, DAC60501_REG_GAIN, (gainReg | 0x0100) & 0xFFFE); // REF-DIV: /2  BUFF-GAIN: *1
            break;
        case MAX_2V5:
            i2c.writeRegister16(deviceId, DAC60501_REG_GAIN, gainReg | 0x0101);  // REF-DIV: /2  BUFF-GAIN: *2
            break;
        case MAX_5V:
            i2c.writeRegister16(deviceId, DAC60501_REG_GAIN, (gainReg & 0xFEFF) | 0x0001); // REF-DIV: /1  BUFF-GAIN: *2
            break;
    }
    uint16_t error = i2c.readRegister16(deviceId, DAC60501_REG_GAIN);

    LOGINFO("reg status: 0x%X", error);
    return (bool) !(error & 0x1);
}

/* Removed float stuff - Haron */
void DAC60501::setVoltage(uint16_t setVoltage) 
{
    if(currentMaxVoltage != 0)
    {
        float adjustment = (float)setVoltage/(float)currentMaxVoltage;
        uint32_t value = 4095.0f * adjustment + 0.5f;
        LOGINFO("Set-Voltage Adjustment: %d, Value: %d", (int)(adjustment * 1000.0f), value);
        value = (value & 0xFFF0) << 4;
        value = ((value & 0xFF) << 8) | ((value & 0xFF00) >> 8);
        i2c.writeRegister16(deviceId, DAC60501_REG_DAC, value);
    }
}
