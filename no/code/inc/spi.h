#ifndef _SPI_H_
#define _SPI_H_

#include <generated/csr.h>
#include "stdint.h"
#include "delay.h"

#define MANUAL_CS

enum SPIDevice
{
	FLASH,
	ICE40,
};

class SPI
{
public:
	SPI(SPIDevice id);
	void init(uint32_t CPOL, uint32_t CPHA);
	void assertCS();
	void releaseCS();
	bool TXBusy();
	void waitTillReady();

	uint8_t readByte();
	void writeByte(uint8_t byte);

private:
	void startTX();

	uint32_t spi_base_addr;
	volatile uint32_t* BUSY_REG = 0;

	/* Control Register Offsets */
	static constexpr uint32_t SPI_TX_OFFSET 		= 0x00;
	static constexpr uint32_t SPI_RX_OFFSET 		= 0x04;
	static constexpr uint32_t SPI_BUSY_OFFSET 		= 0x08;
	static constexpr uint32_t SPI_CONTROL_OFFSET	= 0x0C;
	static constexpr uint32_t SPI_SS_N_OFFSET 		= 0x10;

	/* Interregister Offsets*/
	static constexpr uint32_t SPI_ENABLE_OFFSET = 0;
	static constexpr uint32_t SPI_CPOL_OFFSET 	= 1;
	static constexpr uint32_t SPI_CPHA_OFFSET 	= 2;
};

#endif