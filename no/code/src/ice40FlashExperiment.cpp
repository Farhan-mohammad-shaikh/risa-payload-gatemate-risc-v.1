#include "ice40FlashExperiment.h"

bool ICE40FlashExperiment::init(){	

	uint32_t data_in;
	uint32_t expStartTime = timer1.getTime();

    leds_out_write(0);

	/*Init 8MHz countdown Timer (Overflovs abfter 536s = 8,9Min)*/
	timer1.setUpperLimit(0xFFFFFFFF);
	timer1.setTime(0);
	timer1.start();

	/* Initialize temperure sensors*/
	sensors.temp1.readTempC();
	sensors.temp2.readTempC();
	sensors.temp3.readTempC();

	/* Initialize powermonitor*/
    sensors.ice40_pac.setSampleMode(SPS_1024);
    sensors.ice40_pac.configChannels(BIPOLAR_4V5,BIPOLAR_50mV,BIPOLAR_4V5,BIPOLAR_50mV);
	sensors.ice40_pac.configAccumulator(VSENSE,VSENSE);

	/* Setup ICE40 */
	sensors.dac.setOutputVoltagerange(MAX_2V5); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	sensors.dac.setVoltage(1200);
	sensors.enableICE40OSC(true);
	sensors.enableICE40VCORE(true);
	sensors.enableICE40VIO(true);

	/* ICE40 Programming (ring oscillator: ice40_io_vcore_0 & ice40_io_vcore_1)*/
	SPI ice40_spi(SPIDevice::ICE40);
	ice40_spi.init(1, 0);
	ICE40PROG ice40prog(memory.flash, ice40_spi);
	ice40prog.programm(USERSPACE_OFFSET);
	delayms(10);
	sensors.enableICE40OSC(false);

    ice40_spi.init(0, 0);
    delayms(1);

	//Logging header
	hyperramAddUint8_t('E');
	hyperramAddUint8_t('X');
	hyperramAddUint8_t('P');
	hyperramAddUint8_t(EXPERIMENT_ID);
	hyperramAddUint8_t('#');


	// delayUS(8000000);
	LOGINFO("run will start\n");
	// delayUS(8000000);

    startTime = timer1.getTime();

	return true;
}

ExperimentState ICE40FlashExperiment::run(){

    makeTest();
    LOGINFO("Current Test Case\n");
    return ExperimentState::TEST_FINISHED;

    	
}

bool ICE40FlashExperiment::cleanUp(){
	timer1.stop();
	return true;
}


bool ICE40FlashExperiment::makeTest(){

    uint32_t flashID = readID();
    LOGINFO("Flash ID %x \n", flashID);
    // delayms(20);
	startTime = timer1.getTime();
    read(0x0000A324, 8192);
	endTime = timer1.getTime();
	

	if (endTime <= startTime) {
        elapsedTicks += startTime - endTime;
    } else {
        // Timer has overflowed
        elapsedTicks += (0xFFFFFFFF - startTime) + endTime + 1;
    }

	LOGINFO("StartTime EndTime:: %llu %llu %llu\n", startTime, endTime, elapsedTicks);
    for (uint16_t i = 0; i < 1024; i++)
    {
        LOGINFO("Flash DATA %x \n", flashBuff[i]);
    }
    
    return true;
}


void ICE40FlashExperiment::delayms(uint32_t timeout){
	uint32_t delayTimeout = timer1.getTime()-(timeout*8000);
	while(timer1.getTime() > delayTimeout){}
}

uint32_t ICE40FlashExperiment::runningTime(void){
	return 0xFFFFFFFF-timer1.getTime(); //becaus its a downcounter
}	

void ICE40FlashExperiment::readingSensors(){
    sensors.ice40_pac.refresh();       //refresh before reading values
    hyperramAddUint32_t(runningTime());
   	hyperramAddUint16_t(sensors.ice40_pac.getVoltageCH1Raw());
    hyperramAddUint16_t(sensors.ice40_pac.getVoltageCH2Raw());
    hyperramAddUint16_t(sensors.ice40_pac.getCurrentCH1Raw());
    hyperramAddUint16_t(sensors.ice40_pac.getCurrentCH2Raw());
    hyperramAddUint32_t(sensors.ice40_pac.getPowerCH1());
    hyperramAddUint32_t(sensors.ice40_pac.getPowerCH2());
	hyperramAddUint32_t(sensors.ice40_pac.getAccCurrentCH1());
    hyperramAddUint32_t(sensors.ice40_pac.getAccCurrentCH2());
    hyperramAddUint16_t(sensors.temp1.readTempRaw());
    hyperramAddUint16_t(sensors.temp2.readTempRaw());
    hyperramAddUint16_t(sensors.temp3.readTempRaw()); 
}

void ICE40FlashExperiment::hyperramAddUint8_t(uint8_t data){
    memory.hyperram[ramPos] = data;
    ramPos++;
}

void ICE40FlashExperiment::hyperramAddUint16_t(uint16_t data){
    memory.hyperram[ramPos] = data >> 8;
    ramPos++;
	memory.hyperram[ramPos] = data & 0x00FF;
    ramPos++;
}

void ICE40FlashExperiment::hyperramAddUint32_t(uint32_t data){
	hyperramAddUint16_t(data >> 16);
	hyperramAddUint16_t(data & 0x0000FFFF);
}

void ICE40FlashExperiment::writeAddress(uint32_t address)
{
	/* Address space is 24 Bit */
	ice40_spi.writeByte(address >> 16 & 0xFF);
	ice40_spi.writeByte(address >> 8 & 0xFF);
	ice40_spi.writeByte(address & 0xFF);
	uint8_t Addr1 = address >> 16 & 0xFF;
	uint8_t Addr2 = address >> 8 & 0xFF;
	uint8_t Addr3 = address & 0xFF;

	LOGINFO("Addr: %x %x %x \n", Addr1, Addr2, Addr3);
}

uint32_t ICE40FlashExperiment::readID()
{
	uint32_t result;

	ice40_spi.assertCS();
	ice40_spi.writeByte(0x9F);
	result = ice40_spi.readByte(); 			// 0xC2
	result |= ice40_spi.readByte() << 8;	// 0x28
	result |= ice40_spi.readByte() << 16;	// 0x17
	ice40_spi.releaseCS();
	
	return result;
}

void ICE40FlashExperiment::read(uint32_t address, uint32_t len)
{
	ice40_spi.assertCS();
	ice40_spi.writeByte(0x0B);
	writeAddress(address);

	// Read the Data
	for(uint32_t i = 0; i < len; i++)
	{
		flashBuff[i] = ice40_spi.readByte();
	}

	ice40_spi.releaseCS();

} 