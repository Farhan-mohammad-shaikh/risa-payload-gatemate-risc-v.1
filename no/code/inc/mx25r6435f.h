#ifndef _MX25R6435F_H_
#define _MX25R6435F_H_

#include "spi.h"
#include "logging.h"
#include <stdint.h>
#include <math.h>

#define USERSPACE_OFFSET 1200000 //1048576 /* Aligns with a Sector(4KB) */
#define SECTOR_SIZE 4095
#define BLOCK32K_SIZE 32767
#define BLOCK64K_SIZE 65535

#ifndef MIN(X, Y)
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif

class MX25R6435F
{
public:

	MX25R6435F(SPI& spi);

	/**
	 * @brief Reads a single byte from the given flash address
	 */
	uint8_t readByte(uint32_t address);

	/**
	 * @brief Reads len bytes from the given flash address to dst
	 */
	void read(uint32_t address, uint8_t* dst, uint32_t len); 
	void read(uint32_t address, volatile uint8_t* dst, uint32_t len); 

	/**
	 * @brief Reads the 24Bit ID of the FLASH, has to be 0xC2 0x28 0x17
	 */
	uint32_t readID();

	/**
	 * @brief resets the flash
	 */
	void reset();

	/**
	 * @brief write a single byte to the given address
	 */
	void writeByte(uint32_t address, uint8_t byte);

	/**
	 * @brief write len bytes from data to the given flash address
	 */
	void write(uint32_t address, const uint8_t* data, uint32_t len);

	/**
	 * @brief erases a Sector in Flash(4K)
	 */
	void eraseSector(uint32_t address);

	/**
	 * @brief erases a Sector in Block(32K)
	 */
	void eraseBlock32k(uint32_t address);

	/**
	 * @brief erases a Sector in Block(64K)
	 */
	void eraseBlock64K(uint32_t address);

	/**
	 * @brief Tests the Funcion of the Flash 
	 * 
	 * @retval true if everything works as intended, false if theres an Issue
	 */
	bool testFlash();

private:

	void writeEnable();
	void writeDisable();
	void writeAddress(uint32_t address);
	uint8_t readStatus();
	bool writeInProgress();

	/**
	 * @brief writes into the flash naively without considering page boundaries
	 */
	void naiveWrite(uint32_t address, const uint8_t *data, uint32_t len);

	SPI flash_spi;

	static constexpr uint32_t TEST_SIZE 				= 10;
	static constexpr uint32_t TESTSPACE_OFFSET 			= USERSPACE_OFFSET - SECTOR_SIZE;
	static constexpr uint8_t  TEST_ARRAY[TEST_SIZE] 	= {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	
	static constexpr uint8_t WRITE_IN_PROGRESS_BIT = 1 << 0;

	static constexpr uint8_t NOP 				= 0x00;
	static constexpr uint8_t READ 				= 0x03;
	static constexpr uint8_t READ_STATUS		= 0x05;
	static constexpr uint8_t READ_ID 			= 0x9F;
	static constexpr uint8_t RESET_ENABLE		= 0x66; /* Reset Enable must be executed before executing the RST Command*/
	static constexpr uint8_t RESET 				= 0x99;

	static constexpr uint8_t SECTOR_ERASE 		= 0x20;
	static constexpr uint8_t BLOCK_ERASE_32K 	= 0x52;
	static constexpr uint8_t BLOCK_ERASE_64K 	= 0x03;
	static constexpr uint8_t WRITE_ENABLE 		= 0x06; /* Write Enable must be executed before any Write/Erase Operation*/
	static constexpr uint8_t WRITE_DISABLE		= 0x04;
	static constexpr uint8_t WRITE				= 0x02;
	static constexpr uint32_t PAGE_SIZE 		= 256;
};


#endif /* _MX25R6435F_H_ */