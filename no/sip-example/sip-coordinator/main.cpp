/*
 * Copyright (c) 2024, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <popl.hpp>
#include <unistd.h>
#include <bits/stdc++.h>
#include <vector>
#include <string>

#include <outpost/base/slice.h>
#include <outpost/hal_posix/serial_port.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>
#include <outpost/transport/frame_transport/frame_transport.h>
#include <outpost/sip/coordinator/coordinator.h>
#include <outpost/sip/coordinator/coordinator_packet_receiver.h>
#include <outpost/sip/packet/packet_writer.h>
#include <outpost/sip/packet_transport/packet_transport_wrapper.h>
#include <outpost/rtos/clock.h>

#include <stdio.h>
#include "arr.h"

#include <iostream>

outpost::rtos::SystemClock systemClock;

std::array<uint8_t, 5000> transmitBuffer;
std::array<uint8_t, 5000> receiveBuffer;

outpost::posix::SerialPort sipPort;
outpost::transport::FrameEncoderHdlc frameEncoderHdlc;
outpost::transport::BufferedFrameDecoderHdlc frameDecoderHdlc(outpost::asSlice(receiveBuffer));

outpost::transport::FrameTransportSerial frameTransportSerial(systemClock, sipPort, frameEncoderHdlc, outpost::asSlice(transmitBuffer), frameDecoderHdlc);
outpost::sip::PacketTransportWrapper packetTransportWrapper(frameTransportSerial);

outpost::sip::Coordinator sipCoordinator(packetTransportWrapper);
outpost::sip::CoordinatorPacketReceiver sipReceiver(packetTransportWrapper, sipCoordinator, outpost::support::parameter::HeartbeatSource::default0, 1U);

uint8_t responseData[4096];
outpost::Slice<uint8_t> responseSlice(responseData);

// for string delimiter
std::vector<std::string> split(std::string s, std::string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void startDataWrite(std::vector<uint8_t> &data, uint32_t address, bool write_flash);
void sendAllData(std::vector<uint8_t> &data);
std::vector<uint8_t> dumpData(uint32_t address, bool write_flash, uint32_t len);
uint8_t counter = 0;

int main(int argc, char** argv)
{
	printf("- Start-Up Sender\n");

	popl::OptionParser op("Allowed options");
	auto help_option = op.add<popl::Switch>("h", "help", "produce help message");
	auto port =
			op.add<popl::Value<std::string>>("p", "port", "Serial port for SIP frames", "/dev/ttyS0");
	auto baud =
			op.add<popl::Value<uint32_t>>("b", "baud", "Baudrate for serial port", 115200);
	auto parity =
			op.add<popl::Value<char>>("r", "parity", "Parity for serial port", 'n');
	try
	{
		op.parse(argc, argv);

		if (help_option->count() == 1)
		{
			printf("%s", op.help().c_str());
			return 0;
		}

		printf("SIP with HDLC on port %s, %u baud, %c parity\r\n", port->value().c_str(), baud->value(), parity->value());

		outpost::posix::SerialPort::Parity p = outpost::posix::SerialPort::Parity::none;
		switch(parity->value())
		{
			case 'e':
				p = outpost::posix::SerialPort::Parity::even;
				break;
			case 'o':
				p = outpost::posix::SerialPort::Parity::odd;
				break;
			default:
				break;
		}

		sipPort.open(port->value(), baud->value(), p);

		sipReceiver.start();

		while (1)
		{
			outpost::sip::OperationResult res;
			// wait for command line input:
			while (!std::cin.eof())
			{
				std::cout << "EGSE:> ";
				std::string input;
				std::getline(std::cin, input);

				std::vector<std::string> cmd = split(input, " ");
				input = cmd[0];
				std::vector<std::string> args;
				for(uint32_t i = 1; i < cmd.size(); i++)
				{
					args.push_back(cmd[i]);
				}

				if (input == "exit")
				{
					return 0;
				} 
				else if(input == "cmd0")
				{
					uint8_t payload[] = {0};

					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						0x00, // message counter
						0x00, // type
						0x08, // expected response type
						outpost::Slice<uint8_t>::empty(),
						responseSlice
						);

					if(outpost::sip::OperationResult::success == res)
					{
						if(responseData[0] == 1)
						{
							printf("Got ACK\n");
						}
						else
						{
							printf("Got NACK\n");
						}
					}
					else
					{
						printf("Some error happend\n");
					}

				} 
				else if(input == "cmd1")
				{
					uint8_t payload[] = {0};

					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						0x00, // message counter
						0x01, // type
						0x08, // expected response type
						outpost::Slice<uint8_t>::empty(),
						responseSlice
						);

					if(outpost::sip::OperationResult::success == res)
					{
						if(responseData[0] == 1)
						{
							printf("Got ACK\n");
						}
						else
						{
							printf("Got NACK\n");
						}
					}
				} 
				else if(input == "cmd2")
				{
					/* Parameters to send */
					uint8_t test_id = 0;
					uint16_t duration = 0;
					uint16_t test_size = 0;
					uint16_t test_param = 0;
					uint8_t payload[] = 
					{
						static_cast<uint8_t>(test_id),
						static_cast<uint8_t>(duration), static_cast<uint8_t>((duration >> 8)),
						static_cast<uint8_t>(test_size), static_cast<uint8_t>((test_size >> 8)),
						static_cast<uint8_t>(test_param), static_cast<uint8_t>((test_param >> 8)),
					};

					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						0x00, // message counter
						0x02, // type
						0x08, // expected response type
						outpost::asSlice(payload),
						responseSlice
						);

					if(outpost::sip::OperationResult::success == res)
					{
						if(responseData[0] == 1)
						{
							printf("Got ACK\n");
						}
						else
						{
							printf("Got NACK\n");
						}
					}
				}
				else if(input == "cmd3")
				{
					uint8_t payload[4] = {0xde, 0xad, 0xbe, 0xef};
					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						counter++, // message counter
						0x03, // type
						0x04, // expected response type
						outpost::asSlice(payload),
						responseSlice); 
					if(outpost::sip::OperationResult::success == res)
					{
						printf("Received:");
						for(size_t i = 0; i < 4U; i++)
						{
							printf("%02X ", responseData[i]);
						}
						printf("\r\n");
					}

					if(outpost::sip::OperationResult::crcError == res)
					{
						printf("crcerror\n");
					}
				}
				else if(input == "cmd5")
				{
				  	uint8_t payload[] = {0x00, 0x00, 0x00, 0x00, 100 & 0xFF, 100 >> 8};
					
					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						counter++, // message counter
						0x05, // type
						0x08, // expected response type
						outpost::asSlice(payload),
						responseSlice
						);

					if(outpost::sip::OperationResult::success == res)
					{
						if(responseData[0] == 1)
						{
							printf("Got ACK\n");
						}
						else
						{
							printf("Got NACK\n");
						}
					}
				}
				else if(input == "cmd6")
				{
					uint8_t payload[] = {0x00, 0x00, 0x00, 0x00, 0x00};

					//send your request here
					res = sipCoordinator.sendRequestGetResponseData(
						0x01, // target worker id
						counter++, // message counter
						0x06, // type
						0x08, // expected response type
						outpost::asSlice(payload),
						responseSlice
						);

					if(outpost::sip::OperationResult::success == res)
					{
						if(responseData[0] == 1)
						{
							printf("Got ACK\n");
						}
						else
						{
							printf("Got NACK\n");
						}
					}
				}
				else if(input == "cmd7")
				{
					uint8_t payload[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x02};

					dumpData(0, false, 50);
				}

				switch(res)
				{
				case outpost::sip::OperationResult::transmitError:
					printf("Error on transmit.\r\n");
					break;
				case outpost::sip::OperationResult::responseError:
					printf("Response timeout\r\n");
					break;
				case outpost::sip::OperationResult::workerIdError:
					printf("Wrong worker id\r\n");
					break;
				case outpost::sip::OperationResult::responseTypeError:
					printf("Wrong response type id\r\n");
					break;
				case outpost::sip::OperationResult::success:
				default:
					break;
				}
			}
		}
	}
	catch (std::exception& e)
	{
		printf("Failed: %s\r\n", e.what());
	}
}

std::vector<uint8_t> dumpData(uint32_t address, bool write_flash, uint32_t len)
{
	outpost::sip::OperationResult res;
	uint8_t payload[] = {write_flash, static_cast<uint8_t>((address & 0xFF)), static_cast<uint8_t>((address >> 8)), 
	static_cast<uint8_t>((address >> 16)), static_cast<uint8_t>(len), static_cast<uint8_t>((len >> 8))};
	//send your request here
	res = sipCoordinator.sendRequestGetResponseData(
		0x01, // target worker id
		counter++, // message counter
		0x07, // type
		0x0B, // expected response type
		outpost::asSlice(payload),
		responseSlice
		);

	if(outpost::sip::OperationResult::success == res)
	{
		printf("Received:");
		for(size_t i = 0; i < len; i++)
		{
			printf("%d ", responseData[i]);
		}
		printf("\r\n");
	}

	switch(res)
	{
	case outpost::sip::OperationResult::transmitError:
		printf("Error on transmit.\r\n");
		break;
	case outpost::sip::OperationResult::responseError:
		printf("Response timeout\r\n");
		break;
	case outpost::sip::OperationResult::workerIdError:
		printf("Wrong worker id\r\n");
		break;
	case outpost::sip::OperationResult::responseTypeError:
		printf("Wrong response type id\r\n");
		break;
	case outpost::sip::OperationResult::success:
	default:
		break;
	}
}

void startDataWrite(std::vector<uint8_t> &data, uint32_t address, bool write_flash)
{
	outpost::sip::OperationResult res;
	if(data.size() > 65535)
	{
		return;
	}

	uint8_t payload[] = {write_flash, static_cast<uint8_t>((address & 0xFF)), static_cast<uint8_t>((address >> 8)), 
	static_cast<uint8_t>((address >> 16)), static_cast<uint8_t>(data.size()), static_cast<uint8_t>((data.size() >> 8))};
	printf("INIT with len %d\n", data.size());
	//send your request here
	res = sipCoordinator.sendRequestGetResponseData(
		0x01, // target worker id
		counter++, // message counter
		0x05, // type
		0x08, // expected response type
		outpost::asSlice(payload),
		responseSlice
		);

	if(outpost::sip::OperationResult::success == res)
	{
		if(responseData[0] == 1)
		{
			printf("Got ACK\n");
		}
		else
		{
			printf("Got NACK\n");
		}
	}

	if(outpost::sip::OperationResult::responseError == res)
	{
		printf("Response timeout\r\n");
	}
}

void sendAllData(std::vector<uint8_t> &data)
{
	outpost::sip::OperationResult res;
	const uint16_t max_per_package = 250;
	uint32_t num_packages = data.size() / max_per_package;
	uint32_t leftover_bytes = data.size() % max_per_package;
	uint32_t write_index = 0;

	printf("packages: %d, leftover: %d\n", num_packages, leftover_bytes);
	
	for(uint32_t i = 0; i < num_packages; i++)
	{
		std::vector<uint8_t> cur_vector;
		cur_vector.push_back(max_per_package & 0xFF);
		cur_vector.push_back((max_per_package >> 8) & 0xFF);

		for(uint32_t j = 0; j < max_per_package; j++)
		{
			cur_vector.push_back(data[write_index++]);
		}

		printf("Sending request with cur_vector size %d\n", cur_vector.size());
		//send your request here
		res = sipCoordinator.sendRequestGetResponseData(
			0x01, // target worker id
			counter++, // message counter
			0x06, // type
			0x08, // expected response type
			outpost::asSlice(cur_vector),
			responseSlice
		);

		if(outpost::sip::OperationResult::success == res)
		{
			if(responseData[0] == 1)
			{
				printf("Got ACK\n");
			}
			else
			{
				printf("Got NACK\n");
			}
		}

		switch(res)
		{
		case outpost::sip::OperationResult::transmitError:
			printf("Error on transmit.\r\n");
			break;
		case outpost::sip::OperationResult::responseError:
			printf("Response timeout\r\n");
			break;
		case outpost::sip::OperationResult::workerIdError:
			printf("Wrong worker id\r\n");
			break;
		case outpost::sip::OperationResult::responseTypeError:
			printf("Wrong response type id\r\n");
			break;
		case outpost::sip::OperationResult::success:
		default:
			break;
		}
	}

	if(!leftover_bytes)
	{
		return;
	}

	std::vector<uint8_t> cur_vector;
	cur_vector.push_back(leftover_bytes);
	cur_vector.push_back(0);

	for(uint32_t i = 0; i < leftover_bytes; i++)
	{
		cur_vector.push_back(data[write_index++]);
	}

	//send your request here
	res = sipCoordinator.sendRequestGetResponseData(
		0x01, // target worker id
		counter++, // message counter
		0x06, // type
		0x08, // expected response type
		outpost::asSlice(cur_vector),
		responseSlice
	);

	if(outpost::sip::OperationResult::success == res)
	{
		if(responseData[0] == 1)
		{
			printf("Got ACK\n");
		}
		else
		{
			printf("Got NACK\n");
		}
	}

	switch(res)
	{
	case outpost::sip::OperationResult::transmitError:
		printf("Error on transmit.\r\n");
		break;
	case outpost::sip::OperationResult::responseError:
		printf("Response timeout\r\n");
		break;
	case outpost::sip::OperationResult::workerIdError:
		printf("Wrong worker id\r\n");
		break;
	case outpost::sip::OperationResult::responseTypeError:
		printf("Wrong response type id\r\n");
		break;
	case outpost::sip::OperationResult::success:
	default:
		break;
	}
}