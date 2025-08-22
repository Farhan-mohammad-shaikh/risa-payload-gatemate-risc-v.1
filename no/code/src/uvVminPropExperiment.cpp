#include "uvVminPropExperiment.h"

bool UvVminPropExperiment::init(){
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

	/* Setup GPIOs */
	gpioSetup(GPIO_COUNTER_EN, GPIO_OUTPUT);
	gpioSetup(GPIO_A0, GPIO_OUTPUT);
	gpioSetup(GPIO_A1, GPIO_OUTPUT);
	gpioSetup(GPIO_A2, GPIO_OUTPUT);
	gpioSetup(GPIO_A3, GPIO_OUTPUT);
	gpioSetup(GPIO_A4, GPIO_OUTPUT);
	gpioSetup(GPIO_DATA, GPIO_INPUT);

	/* ICE40 Programming (ring oscillator: ice40_io_vcore_0 & ice40_io_vcore_1)*/
	SPI ice40_spi(SPIDevice::ICE40);
	ice40_spi.init(1, 0);
	ICE40PROG ice40prog(memory.flash, ice40_spi);
	ice40prog.programm(CONFIG_OFFSETS::ICE40_CONFIG);
	delayms(10);
	sensors.enableICE40OSC(false);

	/*Init variables */	
	remainigTestIntervalls = (sizeof(voltageIntervalls) / sizeof(voltageIntervalls[0]));
	remainigRunsPerIntervall = 0;

	//Logging header
	hyperramAddUint8_t('E');
	hyperramAddUint8_t('X');
	hyperramAddUint8_t('P');
	hyperramAddUint8_t(EXPERIMENT_ID);
	hyperramAddUint8_t('#');

	return true;
}

ExperimentState UvVminPropExperiment::run(){
	leds_out_write(0x04);

	setUpTest();
	bool finished = makeTest();

	leds_out_write(0x00);
	if ((remainigTestIntervalls == 0 and remainigRunsPerIntervall == 0) or finished) {
		hyperramAddUint16_t(ramPos);
		hyperramAddUint32_t(runningTime());
		hyperramAddUint8_t('d');
		hyperramAddUint8_t('o');
		hyperramAddUint8_t('n');
		hyperramAddUint8_t('e');
		LOGINFO("Experiment %u finished! time=%lu ram=%lu", EXPERIMENT_ID, runningTime(), ramPos);
		return ExperimentState::TEST_FINISHED;
	}else{
		return ExperimentState::STILL_RUNNING;
	}
}

bool UvVminPropExperiment::cleanUp(){
	timer1.stop();
	return true;
}

void UvVminPropExperiment::setUpTest(){
	const static uint8_t numberofIntervalls = (sizeof(voltageIntervalls) / sizeof(voltageIntervalls[0])); //number of intervalls
	static uint8_t currentIntervall;
	if (remainigRunsPerIntervall == 0) {
		if (remainigTestIntervalls > 0) {			
			currentIntervall = numberofIntervalls - remainigTestIntervalls;
			remainigTestIntervalls--;
			remainigRunsPerIntervall = (voltageIntervalls[currentIntervall][0] - voltageIntervalls[currentIntervall][1]) / voltageIntervalls[currentIntervall][2];
			static bool firstRun = true;
			if (firstRun) {
				firstRun = false;
				voltage = voltageIntervalls[currentIntervall][0];
			}
			else {
				voltage = voltage - voltageIntervalls[currentIntervall][2];
				remainigRunsPerIntervall--;
			}			
		}
	}
	else {
		voltage = voltage - voltageIntervalls[currentIntervall][2];
		remainigRunsPerIntervall--;
	}
}

void UvVminPropExperiment::delayms(uint32_t timeout){
	uint32_t delayTimeout = timer1.getTime()-(timeout*8000);
	while(timer1.getTime() > delayTimeout){}
}
uint32_t UvVminPropExperiment::runningTime(void){
	return 0xFFFFFFFF-timer1.getTime(); //becaus its a downcounter
}	

bool UvVminPropExperiment::makeTest(){
	LOGINFO("Test voltage=%ld", voltage);
	hyperramAddUint16_t(voltage);

	//setup	
	sensors.dac.setVoltage(voltage);
	gpioWrite(GPIO_COUNTER_EN,GPIO_HIGH);
	//sensors.enableICE40VIO(false);
	delayms(300); // for settling Votage

	//currentmessurment acumulator
 	sensors.ice40_pac.refresh();

	//messure
	gpioWrite(GPIO_COUNTER_EN,GPIO_LOW);
	delayms(250);
	gpioWrite(GPIO_COUNTER_EN,GPIO_HIGH);
	delayms(10);

	//readingSensors
	readingSensors();
	delayms(5);
	//readdata
	sensors.dac.setVoltage(1200);
	sensors.enableICE40VIO(true);
	delayms(10);	
	uint32_t data = 0;
	for(uint8_t bit = 0; bit < 20; bit++){		
		gpioWrite(GPIO_A0,(bit>>0) & 0x01);
		gpioWrite(GPIO_A1,(bit>>1) & 0x01);
		gpioWrite(GPIO_A2,(bit>>2) & 0x01);
		gpioWrite(GPIO_A3,(bit>>3) & 0x01);
		gpioWrite(GPIO_A4,(bit>>4) & 0x01);
		delayms(3);
		data = data | ((0x1 & gpioRead(GPIO_DATA))<<bit);
	}

	//add counter value
	LOGINFO("data=%lu\n",data);
	hyperramAddUint32_t(data);

	//add sperator;
	hyperramAddUint8_t('#');

	if(data==0 or data==0xFFFFF){
		return true;
	}else{
		return false;
	}
}	

void UvVminPropExperiment::readingSensors(){
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

void UvVminPropExperiment::hyperramAddUint8_t(uint8_t data){
    memory.hyperram[ramPos] = data;
    ramPos++;
}

void UvVminPropExperiment::hyperramAddUint16_t(uint16_t data){
    memory.hyperram[ramPos] = data >> 8;
    ramPos++;
	memory.hyperram[ramPos] = data & 0x00FF;
    ramPos++;
}

void UvVminPropExperiment::hyperramAddUint32_t(uint32_t data){
	hyperramAddUint16_t(data >> 16);
	hyperramAddUint16_t(data & 0x0000FFFF);
}