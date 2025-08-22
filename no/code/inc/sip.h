#ifndef SIP_H_
#define SIP_H_

#include "stdint.h"

static constexpr uint8_t BOUNDARY = 0x7E;
static constexpr uint8_t ESCAPE = 0x7D;
static constexpr uint8_t BOUNDARY_REPLACE = 0x5E;
static constexpr uint8_t ESCAPE_REPLACE = 0x5D;

static constexpr uint8_t FIRST_DELIMITER_OFFSET = 0;
static constexpr uint8_t LAST_DELIMITER_OFFSET(uint16_t DATA_LENGTH) { return (7 + DATA_LENGTH); }

static constexpr uint8_t COBS_START_OFFSET = 0;
static constexpr uint8_t LENGTH_L_OFFSET = 2;
static constexpr uint8_t LENGTH_H_OFFSET = 1;
static constexpr uint8_t SEQUENCE_OFFSET = 3;
static constexpr uint8_t PAYLOAD_ADDRESS_OFFSET = 4;
static constexpr uint8_t FUNCTION_CODE_OFFSET = 5;
static constexpr uint8_t DATA_OFFSET = 6;
static constexpr uint8_t CRC_OFFSET_L(uint16_t DATA_LENGTH) { return (7 + DATA_LENGTH); }
static constexpr uint8_t CRC_OFFSET_H(uint16_t DATA_LENGTH) { return (6 + DATA_LENGTH); }
static constexpr uint8_t COBS_END_OFFSET(uint16_t DATA_LENGTH) { return (8 + DATA_LENGTH); }
static constexpr uint8_t LENGTH_WITHOUT_DATA = 5;

enum PowerState
{
	OFF,
	ON,
};

enum SIPResult
{
	SUCCESS,
	CRC_ERROR,
	DATA_BIGGER_THAN_BUFFER,
	LENGTH_ERROR,
	SEQUENCE_ERROR,
	UNKNOWN_TYPE_ERROR,
	COBS_ERROR,
};

template <size_t data_size>
struct __attribute__((packed)) SIP
{
	/* ----- Part of the Protocol ----- */
	uint16_t length;
	uint8_t sequence;
	uint8_t pow_or_addr;
	uint8_t code_or_type;
	uint8_t data[data_size];
	uint16_t crc;
	/* -------------------------------- */

	/* ----- Extra Infos for Comfort ----- */
	uint16_t length_without_crc_and_length_field;
	uint16_t data_length;
	/* ----------------------------------- */
};

#include "logging.h"

class SIPUtil
{
public:
	/* Util */
	static uint16_t littleToBigEndian(uint16_t input)
	{
		return (input << 8) | (input >> 8);
	}

	static uint16_t bigToLittleEndian(uint16_t input)
	{
		return (input << 8) | (input >> 8);
	}

	/**
	 * @brief Decodes an array in the HDLC format, meaning seeing
	 * 0x7E as frame boundaries and 0x7D as escape characters
	 * 
	 * Since this changes the given array, make sure to have some extra space left
	 * in the array for the extra bytes to fit
	 * 
	 * @retval amount of bytes the array length decreased
	 */
	static uint32_t hdlc_decode(uint8_t* data, uint16_t len)
	{
		uint8_t copy[len];

		for(uint32_t i = 0; i < len; i++)
		{
			copy[i] = data[i];
		}

		/* Index offset is needed incase the escape character happens
			we skip one byte in the buffer */
		uint32_t index_offset = 0;
		for(uint16_t i = 0; i < len; i++)
		{
			uint8_t cur_byte = copy[i + index_offset];
			uint8_t next_byte = copy[i + index_offset + 1];
			if(cur_byte == ESCAPE)
			{
				switch(next_byte)
				{
					case BOUNDARY_REPLACE:
						data[i] = BOUNDARY;
					break;

					case ESCAPE_REPLACE:
						data[i] = ESCAPE;
					break;
				}
				index_offset++;
			}
			else
			{
				data[i] = cur_byte;
			}
		}

		return index_offset;
	}

	/**
	 * @brief Encodes an array in the HDLC format, meaning seeing
	 * 0x7E as frame boundaries and 0x7D as escape characters
	 * 
	 * Since this changes the given array, make sure to have some extra space left
	 * in the array for the extra bytes to fit
	 * 
	 * @retval amount of bytes the array length increased
	 */
	static uint32_t hdlc_encode(uint8_t* data, uint16_t len)
	{
		uint8_t copy[len];

		for(uint32_t i = 0; i < len; i++)
		{
			copy[i] = data[i];
		}

		/* Index offset is needed incase the escape character happens
			we skip one byte in the buffer */
		uint32_t index_offset = 0;
		for(uint32_t i = 0; i < len; i++)
		{
			uint8_t cur_byte = copy[i];
			
			if(cur_byte == ESCAPE || cur_byte == BOUNDARY)
			{
				data[i + index_offset] = ESCAPE;
				switch(cur_byte)
				{
					case ESCAPE:
						data[i + index_offset + 1] = ESCAPE_REPLACE;
					break;

					case BOUNDARY:
						data[i + index_offset + 1] = BOUNDARY_REPLACE;
					break;
				}
				index_offset++;
			}
			else
			{
				data[i + index_offset] = cur_byte;
			}
		}

		return index_offset;
	}
};

#endif // SIP_H_