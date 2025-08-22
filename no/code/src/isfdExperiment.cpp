#include "isfdExperiment.h"


bool ISFDExperiment::init(){	

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

	/* Setup GPIOs */
    gpioSetup(CLOCK_PIN,GPIO_OUTPUT);
	gpioSetup(LOAD_TEST_PIN,GPIO_OUTPUT);
	gpioSetup(SERIAL_IN_PIN,GPIO_OUTPUT);
	gpioSetup(LOAD_RESULT_PIN,GPIO_OUTPUT);
	gpioSetup(READ_EN_PIN,GPIO_OUTPUT);
	gpioSetup(START_TEST_PIN,GPIO_OUTPUT);
	gpioSetup(RESET_PIN,GPIO_OUTPUT);
	gpioSetup(SERIAL_OUT_PIN,GPIO_INPUT);
	gpioSetup(TC_FLAG_1,GPIO_INPUT);
	gpioSetup(TC_FLAG_2,GPIO_INPUT);

	// Initial output values
	gpioWrite(CLOCK_PIN,GPIO_LOW);
	gpioWrite(LOAD_TEST_PIN,GPIO_LOW);
	gpioWrite(SERIAL_IN_PIN,GPIO_LOW);
	gpioWrite(LOAD_RESULT_PIN,GPIO_LOW);
	gpioWrite(READ_EN_PIN,GPIO_LOW);
	gpioWrite(START_TEST_PIN,GPIO_LOW);
	gpioWrite(RESET_PIN,GPIO_HIGH);

	/* ICE40 Programming (ring oscillator: ice40_io_vcore_0 & ice40_io_vcore_1)*/
	SPI ice40_spi(SPIDevice::ICE40);
	ice40_spi.init(1, 0);
	ICE40PROG ice40prog(memory.flash, ice40_spi);
	ice40prog.programm(USERSPACE_OFFSET+104090);
	delayms(10);
	sensors.enableICE40OSC(false);

	//Logging header
	hyperramAddUint8_t('E');
	hyperramAddUint8_t('X');
	hyperramAddUint8_t('P');
	hyperramAddUint8_t(EXPERIMENT_ID);
	hyperramAddUint8_t('#');


	/*First Log time=0*/
	timeNextEvent = timer1.getTime()-10000000; //Overflow was considered
	LOGINFO("Time %lu\n", experimentTimeS);

	currentTestCase = 0;
	expRunNumber = 0;

	// delayUS(8000000);
	LOGINFO("run will start\n");
	delayUS(8000000);

    initialTimerValue = timer1.getTime();

	return true;
}

ExperimentState ISFDExperiment::run(){

    // if (expRunNumber<TOTAL_EXP_RUN){
    //     if (currentTestCase<TOTAL_TEST_CASE){
    //         makeTest();
    //         // LOGINFO("Current Test Case %lu %x %x \n", currentTestCase, timeBefore, timeAfter);
    //         return ExperimentState::STILL_RUNNING;
    //     } else{
    //         expRunNumber++;
    //         currentTestCase = 0;
    //         LOGINFO("Current Test Case %lu\n", expRunNumber);
    //         return ExperimentState::STILL_RUNNING;
    //     }
        
    // } else{
    //     return ExperimentState::TEST_FINISHED;
    // }


    if (expRunNumber >= TOTAL_EXP_RUN) {
        hyperramAddUint16_t(ramPos);
		hyperramAddUint32_t(runningTime());
		hyperramAddUint8_t('d');
		hyperramAddUint8_t('o');
		hyperramAddUint8_t('n');
		hyperramAddUint8_t('e');
		LOGINFO("Experiment %u finished! time=%lu ram=%lu", EXPERIMENT_ID, runningTime(), ramPos);
		// for (int i = ramPos; i > ramPos-34; i--)
		// {
		// 	LOGINFO("hyperram data: %lu -> %x\n", i, memory.hyperram[i]);
		// }
        return ExperimentState::TEST_FINISHED; // All runs completed
    }

    currentTimerValue = timer1.getTime();

    if (currentTimerValue <= initialTimerValue) {
        elapsedTicks += initialTimerValue - currentTimerValue;
    } else {
        // Timer has overflowed
        elapsedTicks += (0xFFFF - initialTimerValue) + currentTimerValue + 1;
    }

    initialTimerValue = currentTimerValue; // Update initial timer value for the next call

    // Check if 10 minutes have passed
    if (elapsedTicks >= TEN_MINUTES_TICKS) {
        expRunNumber++; // Increment run number after 10 minutes
        currentTestCase = 0; // Reset test case counter
        elapsedTicks = 0; // Reset elapsed ticks for the next 10-minute interval
        LOGINFO("10 minutes passed. Starting run number: %lu\n", expRunNumber);
    }

    if (currentTestCase < TOTAL_TEST_CASE - 1) {
        makeTest(); // Execute the test case
        currentTestCase++;
        LOGINFO("Running test case: %lu\n", currentTestCase);
        return ExperimentState::STILL_RUNNING;
    } else {
        // All test cases completed for the current run
        return ExperimentState::STILL_RUNNING;
    }

    	
}

bool ISFDExperiment::cleanUp(){
	timer1.stop();
	return true;
}


bool ISFDExperiment::makeTest(){

    //readingSensors
	readingSensors();
	delayms(5);
	
    serialWrite(testMatrix[currentTestCase],24);
	flagCheck(currentTestCase,0);
	
	startTest();
    flagCheck(currentTestCase,1);
    
	hyperramAddUint16_t(serialRead(currentTestCase, 9)); 
	flagCheck(currentTestCase,2);
    
	hyperramAddUint8_t(tcInternalFlag[currentTestCase]);
    
	currentTestCase++;
	delayms(5);

	LOGINFO("ResultMSB ResultLSB TCFlugs: %x %x %x\n", memory.hyperram[ramPos], memory.hyperram[ramPos-1], memory.hyperram[ramPos-2]);

	//add sperator;
	hyperramAddUint8_t('#');

	// if(data==0 or data==0xFFFFF){
	// 	return true;
	// }else{
	// 	return false;
	// }

    return true;
}

void ISFDExperiment::flagCheck(uint16_t testNum, uint8_t cp_num){
    tcInternalFlag[testNum] = tcInternalFlag[testNum] | ((0x1 & gpioRead(TC_FLAG_1))<<(2*cp_num));
    tcInternalFlag[testNum] = tcInternalFlag[testNum] | ((0x1 & gpioRead(TC_FLAG_1))<<(2*cp_num+1));
}


void ISFDExperiment::compareFunc(uint16_t testNum){
	// compareMatrix[testNum] = (resultMatrix[testNum] == testResult[testNum])? 1:0;
}

void ISFDExperiment::startTest(void){
		
    gpioWrite(LOAD_RESULT_PIN, GPIO_LOW);
	gpioWrite(READ_EN_PIN, GPIO_HIGH);
	gpioWrite(START_TEST_PIN, GPIO_HIGH);
	gpioWrite(CLOCK_PIN, GPIO_HIGH);
	gpioWrite(CLOCK_PIN, GPIO_LOW);
	gpioWrite(READ_EN_PIN, GPIO_LOW);
	gpioWrite(START_TEST_PIN, GPIO_LOW);

}

uint16_t ISFDExperiment::serialRead(uint16_t testNum, uint8_t length){

	// rep code variables
	uint32_t pinDataA = 0;
	uint32_t pinDataB = 0;
	uint32_t pinDataC = 0;
	uint32_t trueCount = 0;
    uint16_t testResult = 0;

	for (int i = 0; i < length; i++){
		
        // read 1st
		pinDataA = gpioRead(SERIAL_OUT_PIN);

		// read 2nd
		pinDataB = gpioRead(SERIAL_OUT_PIN);

		// read 3rd
		pinDataC = gpioRead(SERIAL_OUT_PIN);
			
		// vote
		trueCount = pinDataA + pinDataB + pinDataC;
		if (trueCount >= 2) {
			testResult = testResult | (1<<i);
		} else {
			testResult = testResult & ~(1<<i);
		}

		// shift data
		gpioWrite(READ_EN_PIN, GPIO_HIGH);
		gpioWrite(LOAD_RESULT_PIN, GPIO_HIGH);
		gpioWrite(CLOCK_PIN, GPIO_HIGH);
		gpioWrite(CLOCK_PIN, GPIO_LOW);
		gpioWrite(READ_EN_PIN, GPIO_LOW);
		gpioWrite(LOAD_RESULT_PIN, GPIO_LOW);		
	}
    return testResult;
}

void ISFDExperiment::serialWrite(uint32_t data, uint8_t length){		
	
    static uint32_t serial_data;

    for (int i = 0; i < length; i++){
	
        gpioWrite(LOAD_TEST_PIN, GPIO_HIGH);
		serial_data = (data & (1 << i)) ? GPIO_HIGH : GPIO_LOW;
			
		gpioWrite(SERIAL_IN_PIN, serial_data);
		gpioWrite(CLOCK_PIN, GPIO_HIGH);
		gpioWrite(CLOCK_PIN, GPIO_LOW);

		gpioWrite(SERIAL_IN_PIN, serial_data);
		gpioWrite(CLOCK_PIN, GPIO_HIGH);
		gpioWrite(CLOCK_PIN, GPIO_LOW);

		gpioWrite(SERIAL_IN_PIN, serial_data);
		gpioWrite(CLOCK_PIN, GPIO_HIGH);
		gpioWrite(CLOCK_PIN, GPIO_LOW);
		gpioWrite(LOAD_TEST_PIN, GPIO_LOW);
	}
	
    gpioWrite(SERIAL_IN_PIN, GPIO_LOW);

}

void ISFDExperiment::delayms(uint32_t timeout){
	uint32_t delayTimeout = timer1.getTime()-(timeout*8000);
	while(timer1.getTime() > delayTimeout){}
}

uint32_t ISFDExperiment::runningTime(void){
	return 0xFFFFFFFF-timer1.getTime(); //becaus its a downcounter
}	

void ISFDExperiment::readingSensors(){
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

void ISFDExperiment::hyperramAddUint8_t(uint8_t data){
    memory.hyperram[ramPos] = data;
    ramPos++;
}

void ISFDExperiment::hyperramAddUint16_t(uint16_t data){
    memory.hyperram[ramPos] = data >> 8;
    ramPos++;
	memory.hyperram[ramPos] = data & 0x00FF;
    ramPos++;
}

void ISFDExperiment::hyperramAddUint32_t(uint32_t data){
	hyperramAddUint16_t(data >> 16);
	hyperramAddUint16_t(data & 0x0000FFFF);
}

