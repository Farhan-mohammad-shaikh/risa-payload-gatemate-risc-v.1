#include "tmp117.h" 

TMP117::TMP117(I2C& i2c_dev, uint8_t id): i2c(i2c_dev), device_id(id)
{
	
}

bool TMP117::init()
{
	/* I2C TEST ERSATZ*/
    i2c.start();
    uint8_t result = i2c.writeAddress(device_id, RD);
    i2c.stop();

    if(result != ACK)
    {
		LOGWARN("NO ACK FROM ADDR 0x%X", device_id);
        //return false;
    }
	
	if(readDeviceID() != CHIP_ID)
	{
		LOGWARN("TMP117 DEVICE ID FOR ADDR 0x%X IS 0x%X INSTEAD OF 0x%X", device_id, readDeviceID(), CHIP_ID);
		//return false;
	}

	/* If all went well, we setup the configuration */
	uint16_t config = 0;

	config |= 0b00 	<< MODE_OFFSET; // Continous Conversion
	config |= 0b000 << CONV_OFFSET; // Fastest ?
	config |= 0b01	<< AVG_OFFSET;	// Weakest Averaging Mode (Beside OFF ofcourse)

	writeConfig(config);

    return true;
}

float TMP117::readTempC()
{
	return ((float)readTempRaw()) * TMP117_RESOLUTION;
}

uint16_t TMP117::readConfig()
{
	return i2c.readRegister16(device_id, CONFIGURATION);
}

bool TMP117::isDataReady()
{
	return readConfig() & (1 << DATA_READY_OFFSET);
}

void TMP117::writeConfig(uint16_t data)
{
	i2c.writeRegister16(device_id, CONFIGURATION, data);
}

void TMP117::setHighLimit(uint16_t data)
{
	i2c.writeRegister16(device_id, THIGH_LIMIT, data);
}

void TMP117::setLowLimit(uint16_t data)
{
	i2c.writeRegister16(device_id, TLOW_LIMIT, data);
}

uint16_t TMP117::readDeviceID()
{
	return i2c.readRegister16(device_id, DEVICE_ID);
}

int16_t TMP117::readTempRaw()
{
	while(!isDataReady());
	return i2c.readRegister16(device_id, TEMP_RESULT);
}

void TMP117::writeTempOffset(uint16_t data)
{
	i2c.writeRegister16(device_id, TEMP_OFFSET, data);
}

