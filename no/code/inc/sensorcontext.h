#ifndef SENSORCONTEXT_H_
#define SENSORCONTEXT_H_

#include "i2c.h"
#include "tmp117.h"
#include "pac1942.h"
#include "dac60501.h"

class SensorContext
{
public:
	SensorContext();

	void setupSensors();

	void enableICE40VIO(bool state);
	void enableICE40VCORE(bool state);
	void enableICE40OSC(bool state);

	I2C i2c0;
	I2C i2c1;

	DAC60501 dac;

	PAC1942 gatemate_pac;
	PAC1942 ice40_pac;

	TMP117 temp1;
	TMP117 temp2;
	TMP117 temp3;
};

#endif // SENSORCONTEXT_H_