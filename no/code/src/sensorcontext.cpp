#include "sensorcontext.h"

SensorContext::SensorContext(): 
/* I2C Constructors*/
i2c0(I2C(I2CBus::BUS0, 400000)), i2c1(I2C(I2CBus::BUS1, 400000)),
/* DAC and PAC Constructors*/
dac(DAC60501(i2c0)), gatemate_pac(PAC1942(i2c1)), ice40_pac(PAC1942(i2c0)),
/* TMP117 Constructors*/
temp1(TMP117(i2c1, 0b1001010)), temp2(TMP117(i2c1, 0b1001011)), temp3(TMP117(i2c0, 0b1001001))
{
	enableICE40VIO(false);
	enableICE40VCORE(false);
	enableICE40OSC(false);
}

void SensorContext::setupSensors()
{
	/* LDO */
	dac.init(0b1001000);
	dac.setOutputVoltagerange(MAX_1V25);

	/* Power Monitor */
	gatemate_pac.init(0b0011111);
	ice40_pac.init(0b0010000);

	/* Temp*/
	temp1.init();
	temp2.init();
	temp3.init();
}

void SensorContext::enableICE40VIO(bool state)
{
	ice40_vio_en_out_write(state);
}

void SensorContext::enableICE40VCORE(bool state)
{
	ice40_core_en_out_write(state);
}

void SensorContext::enableICE40OSC(bool state)
{
	ice40_osc_en_out_write(state);
}
