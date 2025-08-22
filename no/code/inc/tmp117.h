#ifndef TMP117_H_
#define TMP117_H_

#include "i2c.h"
#include "logging.h"

#define TMP117_ADDR_DEFAULT 	0x48 	// TMP117 default i2c address
#define CHIP_ID 				0x0117  // TMP117 default device id from WHOAMI

#define TMP117_RESOLUTION                                                      \
  0.0078125f ///< Scalar to convert from LSB value to degrees C

class TMP117
{
public:
	TMP117(I2C& i2c_dev, uint8_t id);
	bool init();
	void reset();

	/**
	 * @brief Reads the Raw Temperature and converts it to Celcius
	 * this Function doesnt make sure the Data is new
	 * 
	 * @retval Measured Temperature in Celcious 
	 */
	float readTempC();

	/**
	 * @retval true if theres new Data, false if there isnt any new Data
	 */
	bool isDataReady();

	int16_t readTempRaw();
	uint16_t readDeviceID();
	
private:
	
	uint16_t readConfig();
	void writeConfig(uint16_t data);
	void setHighLimit(uint16_t data);
	void setLowLimit(uint16_t data);
	void writeTempOffset(uint16_t data);

	I2C i2c;
	uint8_t device_id;

	static constexpr auto TEMP_RESULT 		= 	0x00; // Temperature data register
	static constexpr auto CONFIGURATION 	= 	0x01; // Configuration register
	static constexpr auto THIGH_LIMIT 		= 	0x02; // High limit set point register
	static constexpr auto TLOW_LIMIT 		= 	0x03; // Low limit set point register
	static constexpr auto TEMP_OFFSET 		= 	0x07; // Temp offset register
	static constexpr auto DEVICE_ID 		= 	0x0F; // Device ID register

	/* Configuartion masks */
	static constexpr auto HIGH_ALRT_OFFSET 		= 	15; // mask to check high threshold alert
	static constexpr auto LOW_ALRT_OFFSET 		= 	14; // mask to check low threshold alert
	static constexpr auto DATA_READY_OFFSET 	= 	13; // mask to check data ready
	static constexpr auto MODE_OFFSET		 	= 	10; // mask to set mode
	static constexpr auto CONV_OFFSET	 		= 	7; 	// conversion cycle bit
	static constexpr auto AVG_OFFSET		 	= 	5; 	// mask to set conversion averaging modes
	static constexpr auto SOFT_RESET_OFFSET		= 	1;	// writing 1 will soft reset with a duration of 2ms
};


#endif // TMP117_H_