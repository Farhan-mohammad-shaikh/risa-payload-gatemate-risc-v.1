#include "../inc/i2c.h"
#include "i2c.h"

#if 1

constexpr uint32_t MICROSECONDS_PER_SECOND = 1000000;
constexpr uint8_t BYTE_BITS = 8;

I2C::I2C(I2CBus bus, uint32_t freq)
{
	switch(bus)
	{
		case I2CBus::BUS0: 
		{
			I2C_W_ADDR_REG = reinterpret_cast<uint32_t*>(CSR_I2C0_W_ADDR);
			I2C_R_ADDR_REG = reinterpret_cast<uint32_t*>(CSR_I2C0_R_ADDR);
			break;
		}
		case I2CBus::BUS1: 
		{
			I2C_W_ADDR_REG = reinterpret_cast<uint32_t*>(CSR_I2C1_W_ADDR);
			I2C_R_ADDR_REG = reinterpret_cast<uint32_t*>(CSR_I2C1_R_ADDR);
			break;
		}
	}
	reset();
	setFrequency(freq);
}

void I2C::reset()
{
	stop();
}

uint8_t I2C::write8(uint8_t byte)
{
	uint8_t bitmask = 0x80;

	for(uint32_t i = BYTE_BITS; i > 0; i--)
	{
		setSDA((byte & bitmask) ? PinState::HIGH : PinState::LOW);
		setSCL(PinState::HIGH);
		bitmask = bitmask >> 1;
		setSCL(PinState::LOW);
	}

	uint8_t ack = getAck();

	return ack;
}

uint8_t I2C::read8()
{
	uint8_t result = 0;

	setOE(PinState::LOW);
	for(uint32_t i = BYTE_BITS; i > 0; i--)
	{
		setSCL(PinState::HIGH);
		result <<= 1;
		result |= (*I2C_R_ADDR_REG & 0x01);
		setSCL(PinState::LOW);
	}

	sendACK();

	return result;
}

void I2C::start()
{
	setOE(PinState::HIGH);
	setSDA(PinState::LOW);
	setSCL(PinState::LOW);
}

void I2C::stop()
{
	setSDA(PinState::LOW);
	setSCL(PinState::HIGH);
	setSDA(PinState::HIGH);
	setOE(PinState::LOW);
}

uint8_t I2C::readRegister8(uint8_t address, uint8_t reg)
{
	uint8_t result;

	start();
	writeAddress(address, WR);
	write8(reg);
	stop();

	start();
	writeAddress(address, RD);
	result = read8();
	stop();

	return result;
}

uint16_t I2C::readRegister16(uint8_t address, uint8_t reg)
{
	uint16_t result;

	start();
	writeAddress(address, WR);
	write8(reg);
	stop();

	start();
	writeAddress(address, RD);
	result = read8() << 8;
	result |= read8();
	stop();

	return result;
}

void I2C::readNBytes(uint8_t address, uint8_t reg, uint16_t bytes, uint8_t* dst)
{
	start();
	writeAddress(address, WR);
	write8(reg);
	stop();

    start();
	writeAddress(address, RD);
	for(uint16_t i = 0; i < bytes; i++) dst[i] = read8();
	stop();
}

bool I2C::writeAddress(uint8_t address, uint8_t rw)
{
	return write8(((address << 1) & 0xFE) | rw);
}

void I2C::writeRegister8(uint8_t address, uint8_t reg, uint8_t byte)
{
	start();
	writeAddress(address, WR);
	write8(reg);
	write8(byte);
	stop();
}

void I2C::writeRegister16(uint8_t address, uint8_t reg, uint16_t bytes)
{
	start();
	writeAddress(address, WR);
	write8(reg);
	write8(bytes & 0xFF);
	write8((bytes >> 8) & 0xFF);
	stop();
}

void I2C::setFrequency(uint32_t freq)
{
	/* Cap result at 1Mhz, since we cant offer more precision */
	if(freq > MICROSECONDS_PER_SECOND)
	{
		i2cdelay_us = 1;
	}
	else
	{
		i2cdelay_us = MICROSECONDS_PER_SECOND / freq;
	}

	/* Needed cause of Operations between a clock, specifically sda changes before scl changes*/
	i2cdelay_us = i2cdelay_us / 2;
	i2cdelay_us = 0;
}

void I2C::sendACK()
{
	setOE(PinState::HIGH);
	setSDA(PinState::LOW);
	setSCL(PinState::HIGH);
	setSCL(PinState::LOW);
}

uint8_t I2C::getAck()
{
	uint8_t result;

	setOE(PinState::LOW);
	setSCL(PinState::HIGH);
	result = (*I2C_R_ADDR_REG & 0x01);
	setSCL(PinState::LOW);
	setOE(PinState::HIGH);

	if(result)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}

void I2C::setSDA(PinState state)
{
	switch(state)
	{
		case PinState::LOW:
			*I2C_W_ADDR_REG &= ~(1 << I2C_SDA_OFFSET);
			break;

		case PinState::HIGH:
			*I2C_W_ADDR_REG |= (1 << I2C_SDA_OFFSET);
			break;

		default:
			//Ignore
			break;
	}

	delayUS(i2cdelay_us);
}

void I2C::setSCL(PinState state)
{
	switch(state)
	{
		case PinState::LOW:
			*I2C_W_ADDR_REG &= ~(1 << I2C_SCL_OFFSET);
			break;

		case PinState::HIGH:
			*I2C_W_ADDR_REG |= (1 << I2C_SCL_OFFSET);
			break;

		default:
			//Ignore
			break;
	}

	delayUS(i2cdelay_us);
}

void I2C::setOE(PinState state)
{
	switch(state)
	{
		case PinState::LOW:
			*I2C_W_ADDR_REG &= ~(1 << I2C_OE_OFFSET);
			break;

		case PinState::HIGH:
			*I2C_W_ADDR_REG |= (1 << I2C_OE_OFFSET);
			break;

		default:
			//Ignore
			break;
	}

	delayUS(i2cdelay_us);
}

#endif