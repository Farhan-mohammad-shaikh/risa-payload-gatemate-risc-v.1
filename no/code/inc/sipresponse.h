#ifndef SIPRESPONSE_H_
#define SIPRESPONSE_H_

#include "sip.h"
#include "crc16.h"

class SIPReponse
{
protected:
	static constexpr size_t BUF_SIZE = 4096;

public:
	SIP<BUF_SIZE> sip_data;

	/**
	 * @brief Built data will be stored as Little Endian
	 */
	SIPResult build(uint8_t sequence, uint8_t power_state, uint8_t response_type, uint8_t* data, uint16_t data_len)
	{
		sip_data.length = LENGTH_WITHOUT_DATA + data_len;
		sip_data.sequence = sequence;
		sip_data.pow_or_addr = power_state;
		sip_data.code_or_type = response_type;

		uint16_t data_length = sip_data.length - LENGTH_WITHOUT_DATA;

		if(data_length > BUF_SIZE)
		{
			return DATA_BIGGER_THAN_BUFFER;
		}

		for(uint32_t i = 0; i < data_length; i++)
		{
			sip_data.data[i] = data[i];
		}

		/* CRC Expects Big Endian, so we convert it for that*/
		sip_data.length = SIPUtil::littleToBigEndian(sip_data.length);
		sip_data.crc = crc16_ccitt(reinterpret_cast<uint8_t*>(&sip_data), SIPUtil::bigToLittleEndian(sip_data.length));
		sip_data.length = SIPUtil::bigToLittleEndian(sip_data.length);

		sip_data.length_without_crc_and_length_field = sip_data.length - sizeof(sip_data.crc) - sizeof(sip_data.length);
		sip_data.data_length = data_length;

		uint16_t size_increase = SIPUtil::hdlc_encode(reinterpret_cast<uint8_t*>(&sip_data), sip_data.length + 2);
		sip_data.length_without_crc_and_length_field += size_increase;
		sip_data.data_length += size_increase;

		return SUCCESS;
	}

	void printAsLog()
	{
		LOGINFO("\nLength = %d\nSequence = %d\nPower_State = %d\nResponse_Type = %d\nCRC = 0x%X",
			sip_data.length,
			sip_data.sequence,
			sip_data.pow_or_addr,
			sip_data.code_or_type,
			sip_data.crc
			);

		uint16_t data_length = sip_data.length - LENGTH_WITHOUT_DATA;

		if(data_length > 0)
		{
			PRINTTEST("SIPResponse: ");

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
};

#endif // SIPRESPONSE_H_