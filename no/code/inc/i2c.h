#ifndef _I2C_H_
#define _I2C_H_

#include <generated/csr.h>
#include <stdint.h>
#include "delay.h"
#include "logging.h"

constexpr uint8_t ACK = 1;
constexpr uint8_t WR = 0;
constexpr uint8_t RD = 1;

enum I2CBus
{
	BUS0,
	BUS1,
};

class I2C
{
public:

	/**
	 * @brief Initializes the I2C Interface and sets a starting frequency
	 * 
	 * @param freq frequency to be set for the interface.
	 */
	I2C(I2CBus bus, uint32_t freq);

	/**
	 * @brief Stops the current Transmission and goes back into the IDLE State
	 */
	void reset();

	/**
	 * @brief Writes a single byte onto the I2C Interface without start or stop signals
	 * 
	 * @param byte to write
	 * 
	 * @retval 1 if ACK was received, else 0
	 */
	uint8_t write8(uint8_t byte);

	/**
	 * @brief Reads a single byte from the I2C Interface without start or stop signals
	 * 
	 * @retval the byte that has been read
	 */
	uint8_t read8();
	
	/**
	 * @brief start and stop signals for the I2C Line
	 */
	void start();
	void repeatedStart();
	void stop();

	/**
	 * @brief Reads a single byte from a register, from the given i2c slave address, uses start and stop signals
	 * 
	 * @param address the i2c slave address to read from
	 * @param reg the register address to read from
	 * 
	 * @retval the byte inside of the given register of the slave
	 */
	uint8_t readRegister8(uint8_t address, uint8_t reg);

	/**
	 * @brief Reads two bytes from a register, from the given i2c slave address, uses start and stop signals
	 * 
	 * @param address the i2c slave address to read from
	 * @param reg the register address to read from
	 * 
	 * @retval the byte inside of the given register of the slave
	 */
	uint16_t readRegister16(uint8_t address, uint8_t reg);

	/**
	 * @brief Reads n bytes from a register, from the given i2c slave address, uses start and stop signals
	 * 
	 * @param address the i2c slave address to read from
	 * @param reg the register address to read from
	 * @param bytes the amount of bytes to read
	 * @param dst the destination of the data
	 *
	 */
	void readNBytes(uint8_t address, uint8_t reg, uint16_t bytes, uint8_t* dst);

	/**
	 * @brief Writes the Address onto the I2C Interface without start or stop signals
	 * 
	 * @param address 7 bit i2c address
	 * @param rw last bit of rw is used to determine the read/write bit for i2c
	 * 
	 * @retval true if ACK was received, else false
	 */
	bool writeAddress(uint8_t address, uint8_t rw);

	/**
	 * @brief Writes a single byte into a register, of the given i2c slave address, uses start and stop signals
	 * 
	 * @param address the i2c slave address to read from
	 * @param reg the register address to write to
	 * @param byte the byte to write into the reg of address
	 */
	void writeRegister8(uint8_t address, uint8_t reg, uint8_t byte);

	/**
	 * @brief Writes a 2 bytes into a register, of the given i2c slave address, uses start and stop signals
	 * 
	 * @param address the i2c slave address to read from
	 * @param reg the register address to write to
	 * @param bytes the bytes to write into the reg of address
	 */
	void writeRegister16(uint8_t address, uint8_t reg, uint16_t bytes);

	/**
	 * @brief set the Frequency of the I2C Interface
	 * 
	 * @param freq new Frequency in Hz, limit is 1Mhz.
	 */
	void setFrequency(uint32_t freq);

private:
	enum class PinState
	{
		LOW,
		HIGH,
	};

	/**
	 * @brief Sends an acknowledgement with an extra clock after the 8th bit
	 */
	void sendACK();

	/**
	 * @brief Sends an extra clock to get the acknowledge after the 8th bit
	 * 
	 * @retval 1 if acknowledgement received, else 0
	 */
	uint8_t getAck();

	/**
	 * @brief set the states of the individual pins to HIGH or LOW
	 */
	void setSDA(PinState state);
	void setSCL(PinState state);
	void setOE(PinState state);

	/* Delay needed to reach the desired frequency*/
	uint32_t i2cdelay_us;

	/* 
	Not using csr.h functions instead we are
	casting the Address of the Write and Read Register for faster access
	*/
	volatile uint32_t* I2C_W_ADDR_REG;
	volatile uint32_t* I2C_R_ADDR_REG; 
	static constexpr uint32_t I2C_SDA_OFFSET = 2;
	static constexpr uint32_t I2C_SCL_OFFSET = 0;
	static constexpr uint32_t I2C_OE_OFFSET = 1;
};

#endif /* _I2C_H_ */