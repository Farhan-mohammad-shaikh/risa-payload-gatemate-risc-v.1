#ifndef EXPTEST_H_
#define EXPTEST_H_

#define EXPERIMENT_ID 3

#include "experiment.h"
#include "timer.h"
#include "logging.h"
#include "gpio.h"

class ExpTest: public Experiment
{
public:
    ExpTest(SensorContext& sensorcontext, ICE40PROG& programmer, MemoryContext& memorycontext, Serial& iceUART) :
	Experiment(sensorcontext, programmer, memorycontext, iceUART), timer1(TimerID::TIMER1){}

	bool init()
	{		
		/*Init Timer 1us resolution countdown*/
		leds_out_write(0);
		timer1.setUpperLimit(0xFFFFFFFF);
		timer1.setTime(0);
		timer1.start();

		/* ICE40 Programming (ring oscillator GATEMATE GPIO: IO_SA_A3 & IO_SA)*/
		SPI ice40_spi(SPIDevice::ICE40);
		ice40_spi.init(1, 0);
		ICE40PROG ice40prog(memory.flash, ice40_spi);
		sensors.dac.setOutputVoltagerange(MAX_2V5); //toDo
		sensors.dac.setVoltage(1200);
		sensors.enableICE40OSC(true);
		sensors.enableICE40VCORE(true);
		sensors.enableICE40VIO(true);
		ice40prog.programm(USERSPACE_OFFSET); //toDo

		sensors.temp1.readTempC();
		sensors.temp2.readTempC();
		sensors.temp3.readTempC();

		//gpioSetup(ice40_io_vio_3,GPIO_OUTPUT);

		/*First Log time=0*/
		timeNextEvent = timer1.getTime()-10000000; //Overflow was considered
		LOGINFO("Time %lu\n", experimentTimeS);


		gpioSetup(ice40_io_vio_0,GPIO_OUTPUT);
		gpioSetup(ice40_io_vio_1,GPIO_OUTPUT);
		gpioSetup(ice40_io_vcore_0,GPIO_OUTPUT);
		gpioSetup(ice40_io_vcore_1,GPIO_OUTPUT);

		while (1)
    	{
			gpioToggle (ice40_io_vio_0);
			gpioToggle (ice40_io_vio_1);
			delayUS(1000000);
			gpioToggle (ice40_io_vcore_0);
			gpioToggle (ice40_io_vcore_1);
			delayUS(1000000);
			LOGINFO("I'm here now");
    	}
		
		return true;
	}

	ExperimentState run()
	{
		/*experimentTimeS calculation*/
		uint32_t timeNow = timer1.getTime();
		if(timeNow<timeNextEvent){
			experimentTimeS++;
			timeNextEvent = timer1.getTime()-10000000; //Overflow was considered
			LOGINFO("Time %lu\n", experimentTimeS);

			/*Log time=(n>0)*/


			/*LED signs of life for debug*/
			static uint32_t status = 0x0;    
			csr_write_simple(status, (CSR_BASE + 0x6800L));
			if(status==0x0){
				status = 0x1; 
				gpioWrite(ice40_io_vio_3, GPIO_HIGH);     
			}else{
				status = 0x0;
				gpioWrite(ice40_io_vio_3, GPIO_LOW); 
			}
		}
		return ExperimentState::STILL_RUNNING;
	}

	bool cleanUp()
	{
		timer1.stop();
		return true;
	}

private:
	Timer timer1;
	uint16_t experimentTimeS = 0;
	uint32_t timeNextEvent = 0;
	uint32_t num_counts = 0;
	uint8_t led_state = 1;
};

#endif // EXPTEST_H_