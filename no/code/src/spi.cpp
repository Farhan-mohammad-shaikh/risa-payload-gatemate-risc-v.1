#include "spi.h"

#if 1

SPI::SPI(SPIDevice id)
{
	switch(id)
	{
		case FLASH: spi_base_addr = CSR_FLASH_BASE; break;
#ifdef CSR_ICE40_BASE
		case ICE40: spi_base_addr = CSR_ICE40_BASE; break;
#endif
	}

	BUSY_REG = reinterpret_cast<uint32_t*>(spi_base_addr + SPI_BUSY_OFFSET);
}

void SPI::init(uint32_t CPOL, uint32_t CPHA)
{
	uint32_t old_content = csr_read_simple(spi_base_addr + SPI_CONTROL_OFFSET);
	uint32_t new_content = old_content | (CPOL << SPI_CPOL_OFFSET) | (CPHA << SPI_CPHA_OFFSET);

	csr_write_simple(new_content, spi_base_addr + SPI_CONTROL_OFFSET);
}

void SPI::assertCS()
{
	csr_write_simple(0, spi_base_addr + SPI_SS_N_OFFSET);
	delayUS(0);
}

void SPI::releaseCS()
{
	csr_write_simple(1, spi_base_addr + SPI_SS_N_OFFSET);
	delayUS(0);
}

void SPI::startTX()
{
	uint32_t old_content = csr_read_simple(spi_base_addr + SPI_CONTROL_OFFSET);
	uint32_t new_content = old_content | (1 << SPI_ENABLE_OFFSET);

	csr_write_simple(new_content, spi_base_addr + SPI_CONTROL_OFFSET);
}

inline bool SPI::TXBusy()
{
	return (*BUSY_REG) & 0x01;
}

inline void SPI::waitTillReady()
{
	while(TXBusy());
}

uint8_t SPI::readByte()
{
	csr_write_simple(0xFF, spi_base_addr + SPI_TX_OFFSET);
	startTX();
	waitTillReady();
	return csr_read_simple(spi_base_addr + SPI_RX_OFFSET) & 0xFF;
}

void SPI::writeByte(uint8_t byte)
{
	waitTillReady();
	csr_write_simple(byte, spi_base_addr + SPI_TX_OFFSET);
	startTX();
}

#endif