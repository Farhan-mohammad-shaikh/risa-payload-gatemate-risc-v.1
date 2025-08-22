#include "mx25r6435f.h"

MX25R6435F::MX25R6435F(SPI& spi) : flash_spi(spi)
{

}

uint8_t MX25R6435F::readByte(uint32_t address)
{
	uint8_t result;

	flash_spi.assertCS();
	flash_spi.writeByte(READ);
	writeAddress(address);
	result = flash_spi.readByte();
	flash_spi.releaseCS();

	return result;
}

void MX25R6435F::read(uint32_t address, uint8_t* dst, uint32_t len)
{
	flash_spi.assertCS();
	flash_spi.writeByte(READ);
	writeAddress(address);

	// Read the Data
	for(uint32_t i = 0; i < len; i++)
	{
		dst[i] = flash_spi.readByte();
	}

	flash_spi.releaseCS();
} 

void MX25R6435F::read(uint32_t address, volatile uint8_t* dst, uint32_t len)
{
	flash_spi.assertCS();
	flash_spi.writeByte(READ);
	writeAddress(address);

	// Read the Data
	for(uint32_t i = 0; i < len; i++)
	{
		dst[i] = flash_spi.readByte();
	}

	flash_spi.releaseCS();
} 

uint32_t MX25R6435F::readID()
{
	uint32_t result;

	flash_spi.assertCS();
	flash_spi.writeByte(READ_ID);
	result = flash_spi.readByte(); 			// 0xC2
	result |= flash_spi.readByte() << 8;	// 0x28
	result |= flash_spi.readByte() << 16;	// 0x17
	flash_spi.releaseCS();
	
	return result;
}

void MX25R6435F::reset()
{
	flash_spi.assertCS();
	flash_spi.writeByte(RESET_ENABLE);
	flash_spi.writeByte(RESET);
	flash_spi.releaseCS();
}

void MX25R6435F::writeByte(uint32_t address, uint8_t byte)
{
	while(writeInProgress());
	writeEnable();

	flash_spi.assertCS();
	flash_spi.writeByte(WRITE);
	writeAddress(address);
	flash_spi.writeByte(byte);
	flash_spi.releaseCS();

	writeDisable();
}

void MX25R6435F::naiveWrite(uint32_t address, const uint8_t* data, uint32_t len)
{
	while(writeInProgress());
	writeEnable();

	flash_spi.assertCS();
	flash_spi.writeByte(WRITE);
	writeAddress(address);

	// Write the Data
	for(uint32_t i = 0; i < len; i++)
	{
		flash_spi.writeByte(data[i]);
	}
	flash_spi.releaseCS();

	writeDisable();
}

void MX25R6435F::write(uint32_t address, const uint8_t* data, uint32_t len)
{
	uint32_t write_index = 0;

	while(write_index < len)
	{
		/* Bytes till page Boundary = Page size - ((address to write) - (last boundary)) */
		uint32_t bytes_till_boundary = PAGE_SIZE - ((address + write_index) - ((address + write_index) & ~0xFF));
		uint32_t bytes_to_write = MIN(len - write_index, bytes_till_boundary);

		naiveWrite(address + write_index, data + write_index, bytes_to_write);

		write_index += bytes_to_write;
	}
}

void MX25R6435F::eraseSector(uint32_t address)
{
	while(writeInProgress());
	writeEnable();

	flash_spi.assertCS();
	flash_spi.writeByte(SECTOR_ERASE);
	writeAddress(address);
	flash_spi.releaseCS();

	writeDisable();
}

void MX25R6435F::eraseBlock32k(uint32_t address)
{
	while(writeInProgress());
	writeEnable();

	flash_spi.assertCS();
	flash_spi.writeByte(BLOCK_ERASE_32K);
	writeAddress(address);
	flash_spi.releaseCS();

	writeDisable();
}

void MX25R6435F::eraseBlock64K(uint32_t address)
{
	while(writeInProgress());
	writeEnable();

	flash_spi.assertCS();
	flash_spi.writeByte(BLOCK_ERASE_64K);
	writeAddress(address);
	flash_spi.releaseCS();

	writeDisable();
}

void MX25R6435F::writeEnable()
{
	flash_spi.assertCS();
	flash_spi.writeByte(WRITE_ENABLE);
	flash_spi.releaseCS();
}

void MX25R6435F::writeDisable()
{
	flash_spi.assertCS();
	flash_spi.writeByte(WRITE_DISABLE);
	flash_spi.releaseCS();
}

void MX25R6435F::writeAddress(uint32_t address)
{
	/* Address space is 24 Bit */
	flash_spi.writeByte(address >> 16 & 0xFF);
	flash_spi.writeByte(address >> 8 & 0xFF);
	flash_spi.writeByte(address & 0xFF);
}

uint8_t MX25R6435F::readStatus()
{
	uint8_t result;

	flash_spi.assertCS();
	flash_spi.writeByte(READ_STATUS);
	result = flash_spi.readByte();
	flash_spi.releaseCS();

	return result;
}

#include "delay.h"

bool MX25R6435F::writeInProgress()
{
	return (readStatus() & WRITE_IN_PROGRESS_BIT);
}

bool MX25R6435F::testFlash()
{
	bool result = true;

	uint8_t before_sector_erase[TEST_SIZE];
	uint8_t after_sector_erase[TEST_SIZE];
	uint8_t after_write[TEST_SIZE];

	LOGTEST("Starting Flash Test");
	LOGTEST("Reading before Sector Erase");
	read(TESTSPACE_OFFSET, before_sector_erase, TEST_SIZE);
	eraseSector(TESTSPACE_OFFSET);
	LOGTEST("Reading after Sector Erase");
	read(TESTSPACE_OFFSET, after_sector_erase, TEST_SIZE);
	write(TESTSPACE_OFFSET, TEST_ARRAY, TEST_SIZE);
	LOGTEST("Reading after Write");
	read(TESTSPACE_OFFSET, after_write, TEST_SIZE);
	LOGTEST("Finished Capturing Data");

	PRINTTEST("Before Erase: ");
	for(uint8_t i = 0; i < TEST_SIZE; i++) PRINTTEST("0x%X ", before_sector_erase[i]);
	PRINTTEST("\n");

	PRINTTEST("After Erase: ");
	for(uint8_t i = 0; i < TEST_SIZE; i++)
	{
		PRINTTEST("0x%X ", after_sector_erase[i]);
		if(after_sector_erase[i] != 0xFF) result = false;
	} 
	PRINTTEST("\n");

	PRINTTEST("After Write: ");
	for(uint8_t i = 0; i < TEST_SIZE; i++)
	{
		PRINTTEST("0x%X ", after_write[i]);
		if(after_write[i] != i) result = false;
	} 
	PRINTTEST("\n");

	if(result) LOGTEST("Test has finished Successfully, the flash may be used now");
	else LOGTEST("Test has failed :(, Flash Status Register and ID: 0x%X. 0x%X", readStatus(), readID());

	return result;
}
