#ifndef EXPERIMENT_H_
#define EXPERIMENT_H_

#include "sensorcontext.h"
#include "memorycontext.h"
#include "ice40prog.h"

enum ExperimentState
{
	TEST_INITIALIZED,
	STILL_RUNNING,
	TEST_FINISHED,
};

class Experiment
{
public:
	Experiment(SensorContext &sensorcontext, ICE40PROG &programmer, MemoryContext &memorycontext, Serial &iceUART) 
	: sensors(sensorcontext), programmer(programmer), memory(memorycontext), iceUART(iceUART) {}
	virtual bool init() = 0;
	virtual ExperimentState run() = 0;
	virtual bool cleanUp() = 0;

protected:
	SensorContext &sensors;
	ICE40PROG &programmer;
	MemoryContext &memory;
	Serial &iceUART;
};

#endif // EXPERIMENT_H_