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

#include <outpost/base/slice.h>
#include <outpost/hal_posix/serial_port.h>
#include <outpost/transport/frame_coder/frame_encoder_hdlc.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder_hdlc.h>
#include <outpost/transport/frame_transport/frame_transport_serial.h>
#include <outpost/transport/frame_transport/frame_transport.h>
#include <outpost/sip/worker/worker.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/sip/packet_transport/packet_transport_wrapper.h>
#include <outpost/rtos/clock.h>
#include <outpost/time/duration.h>

#include <stdio.h>

#include <iostream>

outpost::rtos::SystemClock systemClock;

std::array<uint8_t, 255> transmitBuffer;
std::array<uint8_t, 255> receiveBuffer;

outpost::posix::SerialPort sipPort;
outpost::transport::FrameEncoderHdlc frameEncoderHdlc;
outpost::transport::BufferedFrameDecoderHdlc frameDecoderHdlc(outpost::asSlice(receiveBuffer));

outpost::transport::FrameTransportSerial frameTransportSerial(systemClock, sipPort, frameEncoderHdlc, outpost::asSlice(transmitBuffer), frameDecoderHdlc);
outpost::sip::PacketTransportWrapper packetTransportWrapper(frameTransportSerial);

static constexpr uint8_t workerId = 0x01;
outpost::sip::Worker worker(workerId, packetTransportWrapper);

void printTableHead()
{
    printf("%-15s|%-15s|%-15s|%-15s|%-15s\r\n", "Time", "Worker Id", "Count", "Type Id", "Data");
    printf("=======================================================================================\r\n");
}

void printPacket(const outpost::sip::PacketReader& reader, outpost::time::Duration startTime)
{
    printf("%-15ld|", (systemClock.now() - startTime).timeSinceEpoch().milliseconds());
    printf("%-15u|", reader.getWorkerId());
    printf("%-15u|", reader.getCounter());
    printf("%-15u|", reader.getType());

    if(reader.getPayloadData().getNumberOfElements() > 0)
    {
        for(size_t i =0; i < reader.getPayloadData().getNumberOfElements() && i < 8; i++)
        {
            printf("%02X ", reader.getPayloadData()[i]);
        }
    } else
    {
        printf(" -");
    }

    printf("\r\n");
}

int
main(int argc, char** argv)
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

        uint8_t counter = 0;
        uint8_t packetBuffer[255] = {0};
        uint8_t payloadBuffer[128] = {0};
        outpost::time::Duration startTime = systemClock.now().timeSinceEpoch();

        printTableHead();

        while (1)
        {
            auto res = packetTransportWrapper.receive(outpost::asSlice(packetBuffer), outpost::time::Duration::myriad());
            if(res)
            {
                outpost::sip::PacketReader reader = *res;
                printPacket(reader, startTime);
                if(reader.getWorkerId() == workerId)
                { 
                    switch(reader.getType())
                    {
                        case 0:
                            printf("Received: ");
                            for(uint32_t i = 0; i < reader.getLength(); i++)
                            {
                                printf("0x%X ", reader.getPayloadData()[i]);
                            }
                            printf("\n");
                            break;

                        case 1:
                            worker.sendResponse(counter++, 0x02);
                            break;
                        case 3:
                        {
                            uint16_t length = reader.getPayloadData().getNumberOfElements();
                            for(size_t i = 0;i < length; i ++)
                            {
                                payloadBuffer[i] = reader.getPayloadData()[i];
                            }
                            worker.sendResponse(counter++, 0x04, outpost::asSlice(payloadBuffer).first(length));
                            break;
                        }
                        case 5:
                            worker.sendResponse(counter++, 0x02); // wrong type, will cause error on coordinator.
                            break;
                        default:
                            break;
                    } 
                }
                else
                {
                    // ignore
                }
            }
        }
    }
    catch (std::exception& e)
    {
        printf("Failed: %s\r\n", e.what());
    }
}
