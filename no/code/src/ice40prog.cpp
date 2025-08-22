#include "ice40prog.h"

#if 1

ICE40PROG::ICE40PROG(MX25R6435F& flash, SPI& ice40spi) : flash(flash), ice40spi(ice40spi)
{
	// GPIO Will always be low and only controlled via the OE Register
	ice40_cp_out_write(0);
}

void ICE40PROG::sendConfig()
{
	for(uint32_t i = 0; i < ICE40_FILESIZE; i++)
	{
		ice40spi.writeByte(reserved_ram[i]);
	}
}

void ICE40PROG::programm(uint32_t flash_start_address)
{
	LOGINFO("Started programming ice40");
	LOGINFO("Reading bitfile from flash");
	flash.read(flash_start_address, reserved_ram, ICE40_FILESIZE);

	LOGINFO("Start Resetting ICE40");
	cresetLow();
	ice40spi.assertCS();
	delayUS(10);
	cresetHigh();
	delayUS(1500);

	ice40spi.releaseCS();
	sendDummyBytes(1);
	ice40spi.assertCS();

	LOGINFO("Starting Image Sending");
	// Send the Configuration data
	sendConfig();
	ice40spi.releaseCS();
	sendDummyBytes(16);
	LOGINFO("Done Image Sending");
	
	Timer timer0(TimerID::TIMER0);

	uint32_t start = timer0.getTime();
	bool cdone = false;
	do
	{
		cdone = cdoneRead();
	} while (!cdone && timer0.passed(start) > (100 * MILLISECOND));

	if(cdone)
	{
		LOGINFO("DONE");
	}
	else
	{
		LOGINFO("FAIL");
  }
}

void ICE40PROG::sendDummyBytes(uint32_t bytes)
{
	for(uint32_t i = 0; i < bytes; i++)
	{
		ice40spi.writeByte(0xFF);
	}
}

void ICE40PROG::cresetHigh()
{
	ice40_cp_oe_write(0);
}

void  ICE40PROG::cresetLow()
{
	ice40_cp_oe_write(1);
}

bool ICE40PROG::cdoneRead()
{
	return ice40_cd_in_read();
}


#endif