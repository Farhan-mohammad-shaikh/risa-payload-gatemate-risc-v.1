#include "memorycontext.h"

MemoryContext::MemoryContext(): spi(SPI(SPIDevice::FLASH)), flash(MX25R6435F(spi))
{
}

void MemoryContext::setupMemories()
{
	hyperram_init();
	LOGINFO("setupmemories");
	flash.reset();
	uint32_t id = flash.readID();
	LOGINFO("Flash ID has been read: 0x%x", id);
	//if(!flash.testFlash())
	{
	//	LOGWARN("Flash isnt properly initialized");
	}
}
