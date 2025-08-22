#ifndef MEMORYCONTEXT_H_
#define MEMORYCONTEXT_H_

#include "spi.h"
#include "mx25r6435f.h"
#include "hyperram.h"

#define HYPER_RAM_BASE 0x20000000

class MemoryContext
{
private:
	SPI spi;

public:
	MemoryContext();
	void setupMemories();
	
	MX25R6435F flash;
	volatile uint8_t *hyperram = (uint8_t*)HYPER_RAM_BASE;
};

#endif // MEMORYCONTEXT_H_