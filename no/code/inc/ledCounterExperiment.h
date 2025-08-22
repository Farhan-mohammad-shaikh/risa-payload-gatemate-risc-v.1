#ifndef BLINKEXPERIMENT_H_
#define BLINKEXPERIMENT_H_

#include "experiment.h"
#include "timer.h"
#include "logging.h"

/* Experiment to test the Structure of Experiments */
class LedCounterExperiment: public Experiment
{
public:
    LedCounterExperiment(SensorContext& sensorcontext, ICE40PROG& programmer, MemoryContext& memorycontext, Serial& iceUART) : 
	Experiment(sensorcontext, programmer, memorycontext, iceUART), timer1(TimerID::TIMER1)
    {
        
    }

	bool init()
	{
		leds_out_write(0);
		timer1.setUpperLimit(0xFFFFFFFF);
		timer1.setTime(0);
		timer1.start();
		num_counts = 0;

		timestamp = timer1.getTime();

		return true;
	}

	ExperimentState run()
	{
		if(num_counts > 100)
		{
			return ExperimentState::TEST_FINISHED;
		}

		if(timer1.passed(timestamp) > (SECOND / 20))
		{
			leds_out_write(led_state);
			led_state = (led_state + 1) % (1 << 4);
			num_counts++;

			timestamp = timer1.getTime();
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
	uint32_t timestamp = 0;
	uint32_t num_counts = 0;
	uint8_t led_state = 1;
};

#endif // BLINKEXPERIMENT_H_