/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/parameter/sip.h>
#include <outpost/sip/packet/packet_reader.h>
#include <outpost/sip/packet/packet_writer.h>
#include <outpost/storage/serialize.h>

#include <unittest/harness.h>

class TestPacketReader : public ::testing::Test
{
public:
    TestPacketReader()
    {
    }
};

TEST_F(TestPacketReader, minPacketSize)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    packetWriter.update();

    uint8_t bufferRead[2];
    outpost::Slice<uint8_t> bufferToRead(bufferRead);
    outpost::sip::PacketReader packetReader(bufferToRead);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::lengthErrorTooSmall);
}

TEST_F(TestPacketReader, lengthError)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    packetWriter.update();

    // manipulate length
    bufferWriter[1] = 4;

    uint8_t bufferRead[2];
    outpost::Slice<uint8_t> bufferToRead(bufferRead);
    outpost::sip::PacketReader packetReader(bufferToRead);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::lengthErrorTooSmall);
}

TEST_F(TestPacketReader, crcError)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    packetWriter.update();

    // change crc
    bufferWriter[6] = 0xFF;

    outpost::sip::PacketReader packetReader(bufferPacketWriter);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::crcError);
}

TEST_F(TestPacketReader, readNoPayload)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    packetWriter.update();

    outpost::sip::PacketReader packetReader(bufferPacketWriter);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::success);

    // check length
    ASSERT_EQ(packetReader.getLength(), 5U);
    // check unitId
    ASSERT_EQ(packetReader.getWorkerId(), 1U);
    // check counter
    ASSERT_EQ(packetReader.getCounter(), 2);
    // check type
    ASSERT_EQ(packetReader.getType(), 3);
}

TEST_F(TestPacketReader, readPayload)
{
    if (outpost::sip::parameter::maxPayloadLength < 2)
    {
        // this test requires to check specific payload data
        return;
    }

    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    uint8_t payloadBuffer[2];
    outpost::Slice<uint8_t> payload(payloadBuffer);
    payload[0] = 55;
    payload[1] = 66;
    packetWriter.setPayloadData(payload);

    packetWriter.update();

    outpost::sip::PacketReader packetReader(bufferPacketWriter);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::success);
    auto payloadData = packetReader.getPayloadData();

    // check length
    ASSERT_EQ(packetReader.getLength(), 7U);
    // check unitId
    ASSERT_EQ(packetReader.getWorkerId(), 1U);
    // check counter
    ASSERT_EQ(packetReader.getCounter(), 2);
    // check type
    ASSERT_EQ(packetReader.getType(), 3);
    // check payload
    ASSERT_EQ(payloadData[0], 55);
    // check payload
    ASSERT_EQ(payloadData[1], 66);
}

TEST_F(TestPacketReader, readPayloadWithTooSmallBuffer)
{
    if (outpost::sip::parameter::maxPayloadLength < 2)
    {
        // this test requires to check specific payload data
        return;
    }

    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::sip::PacketWriter packetWriter(outpost::asSlice(bufferWriter));

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    const uint8_t payloadBuffer[] = {"Hello, World!"};
    packetWriter.setPayloadData(outpost::asSlice(payloadBuffer));

    ASSERT_TRUE(packetWriter.update());

    uint8_t bufferReader[outpost::sip::parameter::maxPacketLength];
    const auto actuallyNeededBytes = packetWriter.getReader()->getLength();
    const auto tooSmallReaderBuffer =
            outpost::asSlice(bufferReader)
                    .concatenateCopyFrom(
                            {outpost::asSlice(bufferWriter).first(actuallyNeededBytes - 1)});
    ASSERT_TRUE(tooSmallReaderBuffer);

    outpost::sip::PacketReader packetReader(*tooSmallReaderBuffer);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::lengthErrorTooSmall);
}

TEST_F(TestPacketReader, maxPayload)
{
    uint8_t bufferWriter[outpost::sip::parameter::maxPacketLength];
    outpost::Slice<uint8_t> bufferPacketWriter(bufferWriter);
    outpost::sip::PacketWriter packetWriter(bufferPacketWriter);

    packetWriter.setWorkerId(1);
    packetWriter.setCounter(2);
    packetWriter.setType(3);

    std::array<uint8_t, outpost::sip::parameter::maxPayloadLength> payloadBuffer;
    outpost::Slice<uint8_t> payload = outpost::asSlice(payloadBuffer);
    for (size_t i = 0; i < outpost::sip::parameter::maxPayloadLength; i++)
    {
        payload[i] = i % 256;
    }

    packetWriter.setPayloadData(payload);

    packetWriter.update();

    outpost::sip::PacketReader packetReader(bufferPacketWriter);
    EXPECT_EQ(packetReader.readPacket(), outpost::sip::OperationResult::success);
    auto payloadData = packetReader.getPayloadData();

    // check length
    ASSERT_EQ(packetReader.getLength(), outpost::sip::parameter::maxPayloadLength + 5U);
    // check unitId
    ASSERT_EQ(packetReader.getWorkerId(), 1U);
    // check counter
    ASSERT_EQ(packetReader.getCounter(), 2U);
    // check type
    ASSERT_EQ(packetReader.getType(), 3U);
    // check payload
    for (size_t i = 0; i < outpost::sip::parameter::maxPayloadLength; i++)
    {
        ASSERT_EQ(payloadData[i], i % 256);
    }
}
