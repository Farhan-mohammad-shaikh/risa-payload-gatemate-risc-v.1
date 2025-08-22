#include "sip_handler.h"

SIPHandler::SIPHandler(Serial& obc): obc(obc)
{
}

volatile uint8_t test = 0;

bool SIPHandler::run(SIPCommand* command)
{
	/* I dont know why this is even needed, else the code here doesnt even get executed 
	maybe some compiler optimization bug ??? */
	test = 5;

	/* Return instantly if there arent any Bytes in the Buffer */
	if(!obc.bytesPending())
	{
		return false;
	}

	buffer_index = 0;
	uint8_t first_byte = obc.read();
	buffer[buffer_index++] = first_byte;
	
	/* Get all bytes till the end of the frame is recognized */
	uint8_t cur_byte = 0;
	do
	{
		uint8_t readbyte = obc.read();
		cur_byte = readbyte;
		buffer[buffer_index++] = cur_byte;
	} while (cur_byte != BOUNDARY);
	
	SIPResult result = command->fetch(buffer, buffer_index);
	return true;
}

void SIPHandler::sendResponse(SIPReponse response)
{
	//response.printAsLog();
	obc.write(BOUNDARY);
	/* Convert length to big endian before sending */
	uint16_t be_length = SIPUtil::littleToBigEndian(response.getLength());
	obc.write(reinterpret_cast<uint8_t*>(&be_length), sizeof(be_length));

	/* Send everything but not crc or length */
	obc.write(reinterpret_cast<uint8_t*>(&response.sip_data) + sizeof(response.sip_data.length), 
	response.sip_data.length_without_crc_and_length_field + 2);

	/* Convert crc to big endian before sending */
	uint16_t be_crc = SIPUtil::littleToBigEndian(response.getCRC());
	obc.write(reinterpret_cast<uint8_t*>(&be_crc), sizeof(be_crc));

	obc.write(BOUNDARY);
}

void SIPHandler::sendAck(uint8_t sequence)
{
	SIPReponse response;
	uint8_t response_data[] = {ACK};
	response.build(sequence, POWERED_ON, Response::ACK_NACK_RESPONSE_ID, response_data, 1);
	sendResponse(response);
}

void SIPHandler::sendNack(uint8_t sequence)
{
	SIPReponse response;
	uint8_t response_data[] = {NACK};
	response.build(sequence, POWERED_ON, Response::ACK_NACK_RESPONSE_ID, response_data, 1);
	sendResponse(response);
}
