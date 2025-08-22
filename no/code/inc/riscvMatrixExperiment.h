/*
 *	
 *	RISA Project TU Hamburg 2025
 *  Experiment Testbench for Undervolting Tests on CologneChip GateMate "CCGM1A1-BGA324" FPGA
 *	riscvMatrixExperiment (.h) - Main Experiment File
 *	by Thorbjörn Albrecht - thorbjoern.albrecht@tuhh.de
 *  February 2025
 *
 */

#ifndef RISCVMATRIXEXPERIMENT_H_
#define RISCVMATRIXEXPERIMENT_H_

#define EXPERIMENT_ID 1

// ----- INCLUDES ------------------------------------------------------------------------------------------------------------------------------------------------

#include "experiment.h"
#include "timer.h"
#include "logging.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define CONFIG_SIZE 110000

// ----- TEST VOLTAGE DEFINITIONS --------------------------------------------------------------------------------------------------------------------------------

#define MAX_VOLTAGE         1200
//static const uint16_t matrixVoltageSteps[] = {1200,1100,1050,1025,1000,990,980,970,960,950,945,940,935,930,925,920,915,910,905,900};

// JUST FOR TESTING
static const uint16_t matrixVoltageSteps[] = {1200,950,945,940,935,930,925,920,915,910,905,900};
#define TEST_PER_VOLTAGE 3 //test runs per voltage

// ----- ERROR HANDLING ----------------------------------------------------

#define MAX_TEST_TIME      7000000 //700ms maximal test time

#define ERROR_TIMEOUT       0x01
#define ERROR_TESTID        0x02
#define ERROR_UART          0x03
#define ERROR_DEFAULT       0x04

#define ERROR_SIZE          5

#define MAX_RETRIES         3

// ----- UART DEFINITIONS --------------------------------------------------------------------------------------------------------------------------------------

#define UART_BUFFER_EMPTY   0xFFFFFFFF
#define BUFFER_SIZE         6  				                                // 3 for DUT, 6 for the Testbench
#define UART_TIMEOUT_LIMIT5 5
#define UART_TIMEOUT_LIMIT7 7

// ----- UART FLAG DEFINITIONS ---------------------------------------------------------------------------------------------------------------------------------

#define UART_START_TEST     0x0F
#define UART_TEST_FINISH    0xF0
#define UART_ACK            0x06
#define UART_DATA_H         0xCC
#define UART_DATA_V         0x33
#define UART_DATA_CORRECT   0x69
#define UART_DATA_WRONG     0x96
#define UART_ERROR          0x55
#define UART_TIMEOUT        0x00

// ----- CRC8 DEFINITIONS ---------------------------------------------------------------------------------------------------------------------------------------

#define CRC8_POLY           0x07
#define CRC8_MASK           0x80

// ----- MATRIX DEFINITIONS -------------------------------------------------------------------------------------------------------------------------------------

#define MATRIX_SIZE         32

// ----- SENSOR DEFINITIONS -------------------------------------------------------------------------------------------------------------------------------------
#define SENSORREADINGS_SIZE 10

// ----- UART COMMAND STRUCTURE --------------------------------------------------------------------------------------------------------------------------------

 // UART Command Structure
struct UART_Command{
    uint8_t value;  // Hex value of the command
    uint8_t length; // Length of data associated with the command
};

// UART Flags Structure
struct UART_Flags{
    UART_Command start_test;
    UART_Command test_finish;
    UART_Command ack;
    UART_Command data_h;
    UART_Command data_v;
    UART_Command data_correct;
    UART_Command data_wrong;
    UART_Command error;
};

// ----- RISCV MATRIX EXPERIMENT CLASS -------------------------------------------------------------------------------------------------------------------------

class RiscvMatrixExperiment: public Experiment
{
public:
    RiscvMatrixExperiment(SensorContext& sensorcontext, ICE40PROG& programmer, MemoryContext& memorycontext, Serial& iceUART) : 
	Experiment(sensorcontext, programmer, memorycontext, iceUART), timer1(TimerID::TIMER1)
    {
       
    }

//    ~RiscvMatrixExperiment() {}

    //----- FUNCTION PROTOTYPES -------------------------------------------------------------------------------------------------------------------------------------

    /**
	 * @brief Initalizes Timer, SPI, DAC, Sensors and Flash
	 */
	bool init();

    /**
	 * @brief Runs the RiscvMatrixExperiment
	 */
	ExperimentState run();

    /**
	 * @brief Stops Timer and clears matrixes
	 */
	bool cleanUp();

private:

    bool startedUARTCommunication;
    bool endedUARTCommunication;

    uint32_t HORIZONTALSUM[MATRIX_SIZE];
    uint32_t VERTICALSUM[MATRIX_SIZE];
    
    float preTestReadings[SENSORREADINGS_SIZE];
    float afterTestReadings[SENSORREADINGS_SIZE];
    
    UART_Flags uart_flags; 

    uint8_t size_uartbaseframe;
    uint8_t incoming_data[4];

    uint32_t timeoutTime;
    uint16_t ram_counter;                    //stores current ram writepoint
    uint8_t errorCounter[ERROR_SIZE];       //stores errors and restarts

	uint8_t currentTestID;
    uint8_t data_H_arrivalCounter;
    uint8_t data_V_arrivalCounter;

    bool error;

    Timer timer1;

     /**
	 * @brief Clears Error Counters
	 */
	void clearErrors();

     /**
	 * @brief Clears Data and Sensor Matrixes
	 */
	void clearMatrixes();

     /**
	 * @brief Checks that experiment doesnt take too long
	 */
	bool timeout();

     /**
	 * @brief sets Error Flag and increases indicated Error Counter
	 */
	void logError(uint8_t indicator);

    /**
	 * @brief Gathers all Environmental and Power Data
     * Fixed to a Matrix Size of 9!!!
	 */
	void readEnvironment(float* data);

    /**
	 * @brief Writes data to the RAM
	 */
    void writeDataRAM();

    /**
	 * @brief Calculates and Sets the Voltage in relation to the TestID
	 */
    void setVoltage();

    /**
	 * @brief Sets the Error Flag
	 */
    void setErrorFlag(bool data);

    /**
	 * @brief Clears the Matrix Checksums
	 */
    void clearChecksums();

    /**
	 * @brief Calculates the CRC8 Byte
	 */
    uint8_t calcCRC8(uint8_t* data, uint8_t length);

    /**
	 * @brief Checks the vailidity of the UART message regarding CRC8
	 */
    bool checkCRC8(uint8_t* data, uint8_t length);

    /**
	 * @brief Checks the current UART Buffer
     * @retval returns 0 (success) or -1 (fail)
	 */
    uint8_t validateBuffer(uint8_t *buffer, uint8_t length, uint8_t max_retries);

    /**
	 * @brief Separates a 32-bit data into 4 bytes
	 */
    void splitData(uint32_t data, uint8_t* databytes);

    /**
	 * @brief Combines 4 bytes into one 32-bit uint
     * @retval recombined Data coming in via UART
	 */
    uint32_t combineData(uint8_t* databytes);

    /**
	 * @brief Sends Data via UART
	 */
    void sendUART(UART_Command *command, uint8_t *data);

    /**
	 * @brief Returns received command and stores data in incoming_data
	 */
    uint32_t receiveUART();

};

#endif // RISCVMATRIXEXPERIMENT_H_