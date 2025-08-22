#ifndef DAC60501_H
#define DAC60501_H

#include "i2c.h"
#include <unistd.h>
#include "logging.h"
//#include <bit>

#define DAC60501_REG_DEVID      0x01   // DEVID
#define DAC60501_REG_SYNC       0x02   // SYNC
#define DAC60501_REG_CONFIG     0x03   // CONFIG
#define DAC60501_REG_GAIN       0x04   // GAIN
#define DAC60501_REG_TRIGGER    0x05   // TRIGGER
#define DAC60501_REG_STATUS     0x07   // STATUS
#define DAC60501_REG_DAC        0x08   // DAC

enum MAXVOUT
{
    MAX_1V25 = 1250,
    MAX_2V5 = 2500,
    MAX_5V = 5000
};

class DAC60501
{
public:
    DAC60501(I2C& i2c_dev);
    bool init(int8_t dacId);
    void setVoltage(uint16_t mVolts);
    bool setOutputVoltagerange(enum MAXVOUT vMax);
private:
    I2C i2c;
    int8_t deviceId;
    enum MAXVOUT currentMaxVoltage;
};

#endif
