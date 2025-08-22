#ifndef ICE40FLASHEXPERIMENT_H_
#define ICE40FLASHEXPERIMENT_H_

#include "experiment.h"
#include "timer.h"
#include "logging.h"
#include "gpio.h"

#define EXPERIMENT_ID 4



class ICE40FlashExperiment: public Experiment
{
public:
    ICE40FlashExperiment(SensorContext& sensorcontext, ICE40PROG& programmer, MemoryContext& memorycontext, Serial& iceUART) :
	Experiment(sensorcontext, programmer, memorycontext, iceUART), timer1(TimerID::TIMER1), ice40_spi(SPIDevice::ICE40){}

	bool init();
	ExperimentState run();
	bool cleanUp();

private:
	Timer timer1;
    SPI ice40_spi;
	uint16_t experimentTimeS = 0;
	uint32_t timeNextEvent = 0;
	uint32_t expStartTime = 0;

	uint64_t startTime;
	uint64_t endTime;

	uint32_t initialTimerValue = 0;
	uint32_t currentTimerValue  = 0;
	uint64_t elapsedTicks;

	uint16_t ramPos=0;

    uint8_t flashBuff[8192];

	bool makeTest(void);
	
	void readingSensors(void);
	void delayms(uint32_t timeout);
	uint32_t runningTime(void);
	void hyperramAddUint8_t(uint8_t data);
	void hyperramAddUint16_t(uint16_t data);
	void hyperramAddUint32_t(uint32_t data);

    void writeAddress(uint32_t address);
    uint32_t readID();
    void read(uint32_t address, uint32_t len);

};

#endif // ICE40FLASHEXPERIMENT_H_
