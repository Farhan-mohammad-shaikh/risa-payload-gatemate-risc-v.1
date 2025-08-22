#ifndef UVVMINPROPEXPERIMENT_H_
#define UVVMINPROPEXPERIMENT_H_

#include "experiment.h"
#include "timer.h"
#include "logging.h"
#include "gpio.h"

#define EXPERIMENT_ID 0
#define ICE40_CONFIG Exp0

#define GPIO_COUNTER_EN ice40_io_vio_5
#define GPIO_A0 ice40_io_vio_0
#define GPIO_A1 ice40_io_vio_1
#define GPIO_A2 ice40_io_vio_2
#define GPIO_A3 ice40_io_vio_3
#define GPIO_A4 ice40_io_vio_4
#define GPIO_DATA ice40_io_vcore_1

static const uint16_t voltageIntervalls[][4] = {{1200, 900, 50},{900, 600, 25},{600, 500, 10},{500, 300, 1}};  //{"startVoltage","stopVoltage","voltageStaps"}


class UvVminPropExperiment: public Experiment
{
public:
    UvVminPropExperiment(SensorContext& sensorcontext, ICE40PROG& programmer, MemoryContext& memorycontext, Serial& iceUART) :
	Experiment(sensorcontext, programmer, memorycontext, iceUART), timer1(TimerID::TIMER1){}

	bool init();
	ExperimentState run();
	bool cleanUp();

private:
	Timer timer1;
	uint16_t voltage = 0;
	uint8_t remainigTestIntervalls = 0;
	uint8_t remainigRunsPerIntervall = 0;
	uint16_t ramPos=0;
	
	void setUpTest(void);
	bool makeTest(void);
	void readingSensors(void);
	void delayms(uint32_t timeout);
	uint32_t runningTime(void);		
	void hyperramAddUint8_t(uint8_t data);
	void hyperramAddUint16_t(uint16_t data);
	void hyperramAddUint32_t(uint32_t data);
};

#endif // UVVMINPROPEXPERIMENT_H_