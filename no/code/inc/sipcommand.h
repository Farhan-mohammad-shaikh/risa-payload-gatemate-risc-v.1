#ifndef SIP_COMMAND_H_
#define SIP_COMMAND_H_

#include "stdint.h"
#include "logging.h"
#include "sip.h"
#include "crc16.h"

class SIPCommand
{
protected:
	static constexpr size_t BUF_SIZE = 1024;

private:
	uint8_t last_sequence = 0;

public:
	SIP<BUF_SIZE> sip_data;

	/**
	 * @brief Fetched data will be stored as Little Endian
	 * 
	 * @param buffer of received uart data
	 * @param len length of the frame
	 */
	SIPResult fetch(uint8_t* buf, uint32_t len)
	{
		SIPUtil::hdlc_decode(buf, len);

		/* Retrieve all Data from the Bytes we got */
		sip_data.length = (buf[LENGTH_L_OFFSET] | buf[LENGTH_H_OFFSET] << 8);
		sip_data.data_length = sip_data.length - LENGTH_WITHOUT_DATA;
		sip_data.sequence = buf[SEQUENCE_OFFSET];
		sip_data.pow_or_addr = buf[PAYLOAD_ADDRESS_OFFSET];
		sip_data.code_or_type = buf[FUNCTION_CODE_OFFSET];
		sip_data.length_without_crc_and_length_field = sip_data.length - sizeof(sip_data.crc) - sizeof(sip_data.length);

		if(sip_data.data_length > BUF_SIZE)
		{
			return DATA_BIGGER_THAN_BUFFER;
		}

		if(sip_data.data_length > 0)
		{
			for(uint16_t i = 0; i < sip_data.data_length; i++)
			{
				sip_data.data[i] = buf[i + DATA_OFFSET];
			}
		}

		sip_data.crc = (buf[CRC_OFFSET_L(sip_data.data_length)] | buf[CRC_OFFSET_H(sip_data.data_length)] << 8);

		/* Check for any Errors or Inconsistencies, CRC is calculated with big endian so its converted */
		sip_data.length = SIPUtil::littleToBigEndian(sip_data.length);
		uint16_t calc_crc = crc16_ccitt(reinterpret_cast<uint8_t*>(&sip_data), SIPUtil::bigToLittleEndian(sip_data.length));
		sip_data.length = SIPUtil::bigToLittleEndian(sip_data.length);

		if(sip_data.crc != calc_crc)
		{
			return CRC_ERROR;
		}

		if(buf[LAST_DELIMITER_OFFSET(sip_data.data_length)] != 0x7E)
		{
			return LENGTH_ERROR;
		}

		/* TODO: Overflow beachten*/
		if(sip_data.sequence != last_sequence + 1)
		{
			return SEQUENCE_ERROR;
		}
		last_sequence = sip_data.sequence;

		return SUCCESS;
	}

	void printAsLog()
	{
		LOGINFO("\nLength = %d\nSequence = %d\nPayload_Address = %d\nFunction_Code = %d\nCRC = 0x%X",
			sip_data.length,
			sip_data.sequence,
			sip_data.pow_or_addr,
			sip_data.code_or_type,
			sip_data.crc
			);

		uint16_t data_length = sip_data.length - LENGTH_WITHOUT_DATA;

		if(data_length > 0)
		{
			PRINTTEST("SIPCommand: ");

			for(uint16_t i = 0; i < data_length; i++)
			{
				PRINTTEST("0x%X ", sip_data.data[i]);
			}

			PRINTTEST("\n");
		}
	}

	uint16_t getLength() { return sip_data.length; }
	uint8_t getSequenceNum() { return sip_data.sequence; }
	uint8_t getPayloadAddress() { return sip_data.pow_or_addr; }
	uint8_t getFunctionCode() { return sip_data.code_or_type; }
	uint16_t getCRC() { return sip_data.crc; }
	uint16_t getDataLength() { return sip_data.data_length; }
	uint8_t* getData() { return sip_data.data; }
};

#endif // SIP_COMMAND_H_