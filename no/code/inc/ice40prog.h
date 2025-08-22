#ifndef _ICE40PROG_H_
#define _ICE40PROG_H_

#include <generated/csr.h>
#include "delay.h"
#include "spi.h"
#include "mx25r6435f.h"
#include "logging.h"
#include "memorycontext.h"

#define SIZE_ICE40_CONFIG 104096

enum CONFIG_OFFSETS
{
	//BLINKY_TEST = USERSPACE_OFFSET + 0,
	Exp0 = USERSPACE_OFFSET + 0 * SIZE_ICE40_CONFIG, 	//0x124F80
	Exp1 = USERSPACE_OFFSET + 1 * SIZE_ICE40_CONFIG,	//0x13E620
	Exp2 = USERSPACE_OFFSET + 2 * SIZE_ICE40_CONFIG,	//0x157CC0
};

/* Last Megabyte is reserved for this code */
#define RESERVED_HYPERRAM 7000000
#define RESERVED_SIZE 1000000

#define ICE40_FILESIZE 104090

class ICE40PROG
{
public:
	/**
	 * @param flash a Driver to the Flash of the device
	 * @param ice40spi SPI Connection to the ICE40 Programming Pins
	 */
	ICE40PROG(MX25R6435F& flash, SPI& ice40spi);

	/**
	 * @brief Programms the ICE40 with the data in the flash given by
	 * the flash_start_address and the length of the bitstream following that address
	 */
	void programm(uint32_t flash_start_address);

private:
	void sendConfig();
	void sendZeroes(uint32_t zeroes);
	void sendDummyBytes(uint32_t bytes);
	void cresetHigh();
	void cresetLow();

	bool cdoneRead();

	MX25R6435F flash;
	SPI ice40spi;

	volatile uint8_t *reserved_ram = (uint8_t*)(HYPER_RAM_BASE + RESERVED_HYPERRAM);
	static constexpr uint32_t dummy_bits = 49;
	static constexpr uint32_t dummy_bytes = (dummy_bits / 8) + 1;
	static constexpr uint32_t reads_per_run = 8;
};

#endif /* _ICE40PROG_H_ */