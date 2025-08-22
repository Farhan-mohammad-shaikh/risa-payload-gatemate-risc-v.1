/*
 *	
 *	RISA Project TU Hamburg 2025
 *  Experiment Testbench for Undervolting Tests on CologneChip GateMate "CCGM1A1-BGA324" FPGA
 *	riscvMatrixExperiment (.cpp) - Main Experiment File
 *	by Thorbjörn Albrecht - thorbjoern.albrecht@tuhh.de
 *  February 2025
 *
 */

#include "riscvMatrixExperiment.h"
#include "serial.h"

bool RiscvMatrixExperiment::init()
{	
    memset(HORIZONTALSUM,0,sizeof(HORIZONTALSUM));
    memset(VERTICALSUM,0,sizeof(VERTICALSUM));
    memset(preTestReadings,0,sizeof(preTestReadings));
    memset(afterTestReadings,0,sizeof(afterTestReadings));

    //NEU
    startedUARTCommunication = false;
    endedUARTCommunication = false;

    size_uartbaseframe = 2;  // Commando + CRC8 (Data Length added in sendUART)
    memset(incoming_data,0,sizeof(incoming_data));
    data_H_arrivalCounter = 0;
    data_V_arrivalCounter = 0;
    error = false;
    currentTestID = 0;
    timeoutTime = 0;
    ram_counter = 0;
    memset(errorCounter,0,sizeof(errorCounter));

    uart_flags = {
        .start_test = { .value = UART_START_TEST, .length = 1 },
        .test_finish = { .value = UART_TEST_FINISH, .length = 1 },   
        .ack = { .value = UART_ACK, .length = 1 },   
        .data_h = { .value = UART_DATA_H, .length = 4 },        
        .data_v = { .value = UART_DATA_V, .length = 4 },   
        .data_correct = { .value = UART_DATA_CORRECT, .length = 1 },
        .data_wrong = { .value = UART_DATA_WRONG, .length = 1 },
        .error = { .value = UART_ERROR, .length = 1 },                                      
    };

    /*Init Timer 1us resolution countdown*/
    timer1.setUpperLimit(0xFFFFFFFF);
    timer1.setTime(0xFFFFFFFF);
    timer1.start();    

    /* ICE40 Programming ()*/
    SPI ice40_spi(SPIDevice::ICE40);
    ice40_spi.init(1, 0);

    sensors.dac.setOutputVoltagerange(MAX_2V5);
    sensors.dac.setVoltage(1200);
    sensors.enableICE40OSC(true);
    sensors.enableICE40VCORE(true);
    sensors.enableICE40VIO(true);

    //programmer.programm(USERSPACE_OFFSET + EXPERIMENT_ID * CONFIG_SIZE);
    programmer.programm(CONFIG_OFFSETS::Exp1);
    // Firmware zweimal im Flash ablegen, falls eins kaputt geht???

    /* Save Experiment ID to RAM */

    memory.hyperram[ram_counter] = EXPERIMENT_ID;
    ram_counter++;

    //flush buffer
    while(!iceUART.isEmpty()){
        iceUART.readNonBlocking(); 
    }    

    LOGINFO("RiscvMatrixExperiment joins the party!\n");
    return true;
}

ExperimentState RiscvMatrixExperiment::run()
{
    //LOGINFO("DB0:Current Test ID is %d\n", currentTestID);
    if(!startedUARTCommunication){
        // First Start Settings - only done once at the beginning
        error = false;  									// Clear the error flag at the start of a new test 
        endedUARTCommunication = false;                     // Reset ended UART Communication Indicator                            
        setVoltage();
        readEnvironment(preTestReadings);  
        if(currentTestID>11){
            LOGINFO("DB2:Voltage Set and EnvironmentData Read\n");
        }   
        sendUART(&uart_flags.start_test, &currentTestID);
        if(currentTestID>11){
            LOGINFO("DB3: Start Test sent\n");
        }       
        startedUARTCommunication = true;
        return ExperimentState::STILL_RUNNING;
    }
    if(currentTestID>11){
        LOGINFO("DB1:New Run entered\n");
    }  
    
    timeoutTime = timer1.getTime() - MAX_TEST_TIME;               // Save the start time of the test
    while(!endedUARTCommunication && !error && !timeout()){
        if(!iceUART.isEmpty()){
            if(currentTestID>11){
                LOGINFO("DB4: UART all checks passed\n");
            }
            uint32_t message = receiveUART();

            if(currentTestID>11){
                LOGINFO("DB5: message from receive UART\n");
            }

            switch (message){                    
                case UART_TEST_FINISH:
                    //Check if TEST ID is correct
                    if(currentTestID>11){
                        LOGINFO("DB6: Message is UART_TEST_FINISH\n");
                    }
                    if (currentTestID == incoming_data[0]){
                        sendUART(&uart_flags.ack, &currentTestID);
                    } else {
                        logError(ERROR_TESTID);                                               // Set Error Flag
                    }
                    break;
                    
                case UART_DATA_H:
                    //must be smaller MATRIX_SIZE-1, so the switch to Data_V happens without Dataloss
                    if(currentTestID>11){
                        LOGINFO("DB7: Message is UART_DATA_H\n");
                    }
                    if (data_H_arrivalCounter < (MATRIX_SIZE - 1)){ 
                        HORIZONTALSUM[data_H_arrivalCounter] = combineData(incoming_data);
                        data_H_arrivalCounter++;
                    } else {
                        HORIZONTALSUM[data_H_arrivalCounter] = combineData(incoming_data);    //just for the (MATRIXSIZE - 1)th case
                        data_H_arrivalCounter = 0;
                    }
                    break;
                
                case UART_DATA_V:
                    //must be smaller MATRIXSIZE-1, so the data_arrivalCounter gets reset for later
                    if(currentTestID>11){
                        LOGINFO("DB8: Message is UART_DATA_V\n");
                    }
                    if (data_V_arrivalCounter < (MATRIX_SIZE - 1)){ 
                        VERTICALSUM[data_V_arrivalCounter] = combineData(incoming_data);
                        data_V_arrivalCounter++;
                    } else {
                        VERTICALSUM[data_V_arrivalCounter] = combineData(incoming_data);      //just for the (MATRIXSIZE - 1)th case
                        data_V_arrivalCounter = 0;
                        endedUARTCommunication = true;                                         //Data is processed, so no new message is read
                    }
                    break;
                
                case UART_ERROR:
                    if(currentTestID>11){
                        LOGINFO("DB9: Message is UART_ERROR\n");
                    }    
                    logError(ERROR_UART);                                                 // Set Error Flag
                    break;
                    
                default:
                    logError(ERROR_DEFAULT); 						// Set Error Flag
                break;  
            }   
        }
    }
    if(currentTestID>11){
        LOGINFO("DB10: UART handling done\n");
    }

    if (error != true){

        if(currentTestID>11){
            LOGINFO("DB11:No Error indicated\n");
        }

        readEnvironment(afterTestReadings);
        sendUART(&uart_flags.data_correct, &currentTestID);
        writeDataRAM();
        if(currentTestID == ((sizeof(matrixVoltageSteps) / sizeof(matrixVoltageSteps[0]))*TEST_PER_VOLTAGE)-1 && cleanUp()){
            LOGINFO("DBX: Max TestID reached, printing RAM now:\n");
            LOGINFO("ram_counter=%lu",ram_counter);
            for(uint16_t pos=0; pos<ram_counter; pos++){
                LOGINFO("%u", memory.hyperram[pos]);
            }           
            
            return ExperimentState::TEST_FINISHED;
        } else {
            if(currentTestID>11){
                LOGINFO("DB12: Increasing TestID\n");
            }
            currentTestID++;
            return ExperimentState::STILL_RUNNING;
        }
    } else {
        if(currentTestID>11){
            LOGINFO("DB13:Error was indicated\n");
            LOGINFO("TIMEOUT=%d, TESTID=%d, UART=%d\n", errorCounter[0],errorCounter[1],errorCounter[2]);
        }
        readEnvironment(afterTestReadings);
        sendUART(&uart_flags.data_wrong, &currentTestID);
        writeDataRAM();
        if( errorCounter[4] >= MAX_RETRIES){
            // Abort Test
            return ExperimentState::TEST_FINISHED;
        }
        errorCounter[4]++;          //increase Restart Counter
        //reflash Firmware
        //programmer.programm(USERSPACE_OFFSET + EXPERIMENT_ID*CONFIG_SIZE);
        sensors.dac.setVoltage(1200);
        programmer.programm(CONFIG_OFFSETS::Exp1);
        return ExperimentState::STILL_RUNNING;
        //Start another test without increase in TestID
    }
}


bool RiscvMatrixExperiment::cleanUp()
{
	timer1.stop();
    clearErrors(); 
    clearMatrixes();
	return true;
}

void RiscvMatrixExperiment::clearErrors(){
    for (int i = 0; i < ERROR_SIZE; i++){
        errorCounter[i] = 0; 
    }
}

void RiscvMatrixExperiment::clearMatrixes(){
    for (int i = 0; i < MATRIX_SIZE; i++){
        VERTICALSUM[i] = 0;
        HORIZONTALSUM[i] = 0;
    }
    for (int i = 0; i < SENSORREADINGS_SIZE; i++){
        preTestReadings[i] = 0;
        afterTestReadings[i] = 0;
    }
}

bool RiscvMatrixExperiment::timeout(){

    if(timeoutTime > timer1.getTime()){
        logError(ERROR_TIMEOUT);
        return true;
    } else {
        return false;
    }
}

void RiscvMatrixExperiment::logError(uint8_t indicator){
    switch(indicator){

        case ERROR_TIMEOUT:
            errorCounter[0]++;
        break;

        case ERROR_TESTID:
            errorCounter[1]++;
        break;

        case ERROR_UART:
            errorCounter[2]++;
        break;

        case ERROR_DEFAULT:
            errorCounter[3]++;
        break;
    }

    setErrorFlag(true);
}

void RiscvMatrixExperiment::readEnvironment(float* data){
    sensors.ice40_pac.refresh();       //refresh before reading values
    data[0] = timer1.getTime();
    data[1] = sensors.ice40_pac.getVoltageCH1();
    data[2] = sensors.ice40_pac.getVoltageCH2();
    data[3] = sensors.ice40_pac.getCurrentCH1();
    data[4] = sensors.ice40_pac.getCurrentCH2();
    data[5] = sensors.ice40_pac.getPowerCH1();
    data[6] = sensors.ice40_pac.getPowerCH2();
    data[7] = sensors.temp1.readTempC();
    data[8] = sensors.temp2.readTempC();
    data[9] = sensors.temp3.readTempC(); 

}

void RiscvMatrixExperiment::writeDataRAM(){

    // Write Test ID
    memory.hyperram[ram_counter] = currentTestID;
    
    // Write first matrix
    for (int i = 0; i < MATRIX_SIZE; i++) {
        memory.hyperram[ram_counter] = HORIZONTALSUM[i];
        ram_counter++;
    }
    
    // Write second matrix
     for (int i = 0; i < MATRIX_SIZE; i++) {
        memory.hyperram[ram_counter] = VERTICALSUM[i];
        ram_counter++;
    }
    
    // Write pre Test sensor readings
    for (int i = 0; i < SENSORREADINGS_SIZE; i++) {
        memory.hyperram[ram_counter] = preTestReadings[i];
        ram_counter++;
    }

    // Write after Test sensor readings
    for (int i = 0; i < SENSORREADINGS_SIZE; i++) {
        memory.hyperram[ram_counter] = afterTestReadings[i];
        ram_counter++;
    }

    for (int i = 0; i < ERROR_SIZE; i++) {
        memory.hyperram[ram_counter] = errorCounter[i];
        ram_counter++;
    }
    
    // Add newline (0x0A) at the end
    memory.hyperram[ram_counter] = 0x0A;
    
}

void RiscvMatrixExperiment::setVoltage(){

    //uint32_t groupVoltage = currentTestID / 3;
    //uint32_t voltage = MAX_VOLTAGE - groupVoltage * VOLTAGE_STEP;

    uint32_t voltage =  matrixVoltageSteps[currentTestID / TEST_PER_VOLTAGE];
    LOGINFO("Voltage=%lu", voltage);
    sensors.dac.setVoltage((voltage > MAX_VOLTAGE) ? MAX_VOLTAGE : voltage);
}

void RiscvMatrixExperiment::setErrorFlag(bool data){
    // Set Error Flag
    error = data;
}

void RiscvMatrixExperiment::clearChecksums(){

    for (int i = 0; i < MATRIX_SIZE; i++){
        HORIZONTALSUM[i] = 0;
        VERTICALSUM[i] = 0;
    }
}

uint8_t RiscvMatrixExperiment::calcCRC8(uint8_t* data, uint8_t length) {
    uint8_t crc = 0;
    for (int i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & CRC8_MASK) {
                crc = (crc << 1) ^ CRC8_POLY;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool RiscvMatrixExperiment::checkCRC8(uint8_t* data, uint8_t length) {
    if (length < 3) {
        return false;                                               // Need at least a header and one data byte and one CRC byte
    }
    // Rearrange data: move the CRC byte (data[1]) to the end
    uint8_t rearranged[length];
    rearranged[0] = data[0];                                        // Keep the first byte as is
    for (int i = 1; i < length - 1; i++) {
        rearranged[i] = data[i + 1];                                // Shift bytes from index 2 onwards
    }
    rearranged[length - 1] = data[1];                               // Move CRC byte to the end

    // Calculate CRC-8 over the rearranged data excluding the CRC byte
    if (calcCRC8(rearranged, length - 1) == rearranged[length - 1]){
        return true;
    } else {
        return false;
    }
}

uint8_t RiscvMatrixExperiment::validateBuffer(uint8_t *buffer, uint8_t length, uint8_t max_retries) {
    uint8_t retryCount = 0;
    while (checkCRC8(buffer, length) != true) {
        // Shift buffer content
        for (int i = 0; i < length - 1; i++) {
            buffer[i] = buffer[i + 1];
        }
        buffer[length - 1] = iceUART.readNonBlocking();                             // Read the next byte
        retryCount++;
        if (retryCount >= max_retries) {
            return -1;                                              // Return failure
        }
    }
    return 0; // Success
}

void RiscvMatrixExperiment::splitData(uint32_t data, uint8_t* databytes) {
    databytes[0] = (data >> 0) & 0xFF;
    databytes[1] = (data >> 8) & 0xFF;
    databytes[2] = (data >> 16) & 0xFF;
    databytes[3] = (data >> 24) & 0xFF;
}

uint32_t RiscvMatrixExperiment::combineData(uint8_t* databytes) {
    uint32_t data = 0;
    data |= databytes[0] << 0;
    data |= databytes[1] << 8;
    data |= databytes[2] << 16;
    data |= databytes[3] << 24;
    return data;
}

void RiscvMatrixExperiment::sendUART(UART_Command *command, uint8_t *data) {

    uint8_t size_uartframe = size_uartbaseframe + command->length;  // saves the size of the frame
    uint8_t byte[size_uartframe];                                   // byte holds complete frame with command, data and CRC
    uint8_t mergedData[command->length + 1];                        // mergedData holds command and data, withou whitespace
    byte[0] = command->value;                                       // reads current command
    mergedData[0] = command->value;

    // Calculate CRC, handle NULL
    for (int i = 0; i < command->length; i++) {
        byte[i + size_uartbaseframe] = (data) ? data[i] : 0;        // Set data or 0 if no data
        mergedData[i+1] = (data) ? data[i] : 0;                     // Set data or 0 if no data
    }
    // Set CRC if data is available
    if (byte && command->length > 0) {
        byte[1] = calcCRC8(mergedData, command->length + 1);            // command->length is the length of connected data
    } else {
        byte[1] = 0;                                                // Set CRC to 0 if no data
    } 
    iceUART.write(byte, size_uartframe);
}

uint32_t RiscvMatrixExperiment::receiveUART() {
    uint8_t header = 0;
    uint8_t uartDataBuffer[BUFFER_SIZE] = {0};
    header = iceUART.readNonBlocking(); 
    while(  header != uart_flags.start_test.value &&
    	    header != uart_flags.test_finish.value &&
            header != uart_flags.ack.value && 
            header != uart_flags.data_correct.value && 
            header != uart_flags.data_wrong.value && 
            header != uart_flags.data_h.value && 
            header != uart_flags.data_v.value && 
            header != uart_flags.error.value &&
            timeout() != true
            ){
            if(!iceUART.isEmpty()){
                header = iceUART.readNonBlocking();                                        // wait for a valid header
            }
    }
    switch (header) {
        //case UART_START_TEST:
        case UART_TEST_FINISH:
        case UART_ACK:
        case UART_DATA_CORRECT:
        case UART_DATA_WRONG:
            uartDataBuffer[0] = header;
            for (int i = 1; i < 3; i++) {
                uartDataBuffer[i] = iceUART.readNonBlocking(); 
            }
            if (validateBuffer(uartDataBuffer, 3, UART_TIMEOUT_LIMIT5) < 0) {
                logError(ERROR_UART);
            }
            incoming_data[0] = uartDataBuffer[2];                   // store the data (here TestID) in incoming_data
            return header;
        break;
          
        case UART_DATA_H:
        case UART_DATA_V:
             // Validate 6-byte frames
            uartDataBuffer[0] = header;
            for (int i = 1; i < 6; i++) {
                uartDataBuffer[i] = iceUART.readNonBlocking(); 
            }
            if (validateBuffer(uartDataBuffer, 6, UART_TIMEOUT_LIMIT7) < 0) {
                logError(ERROR_UART);                  // Too many retries
            }
            for (int i = 2; i < 6; i++) {
                incoming_data[i - 2] = uartDataBuffer[i];           //store the data (here 4 Data Bytes) in incoming_data
            }
            return header;
        break;
        
        case UART_ERROR:
        
        default:        
            logError(ERROR_UART);
            return header; 
    }
}
