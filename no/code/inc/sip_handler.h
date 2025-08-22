#ifndef SIP_HANDLER_H_
#define SIP_HANDLER_H_

#include "serial.h"
#include "sip.h"
#include "sipcommand.h"
#include "sipresponse.h"

#define SIP_HANDLER_BUF_SIZE 512
#define ACK 1
#define NACK 0

#define POWERED_ON 1
#define POWERED_OFF 0

enum Command
{
	NOTHING = -1,
	RISA_INIT_COMMAND_ID = 0,
	SHUTDOWN_COMMAND_ID = 1,
	TEST_START_COMMAND_ID = 2,
	TEST_STATUS_COMMAND_ID = 0x0C,
	TESTDATA_COMMAND_ID = 3,
	HOUSEKEEPINGDATA_COMMAND_ID = 4,
	MEMORY_WRITE_INIT_COMMAND_ID = 5,
	MEMORY_WRITE_DATA_COMMAND_ID = 6,
	MEMORY_DUMP_COMMAND_ID = 7,
};

enum Response
{
	ACK_NACK_RESPONSE_ID = 8,
	TESTDATA_RESPONSE_ID = 0x0E,
	TEST_STATUS_RESPONSE_ID = 0x0D,
	HOUSEKEEPINGDATA_RESPONSE_ID = 10,
	MEMORY_DUMP_RESPONSE_ID = 11,
};

class SIPHandler
{
public:
	SIPHandler(Serial& obc);

	bool run(SIPCommand* command);
	void sendResponse(SIPReponse response);
	void sendAck(uint8_t sequence);
	void sendNack(uint8_t sequence);
private:

	Serial& obc;
	uint8_t buffer[SIP_HANDLER_BUF_SIZE];
	uint32_t buffer_index = 0;
};


#endif // SIP_HANDLER_H_
