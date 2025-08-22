/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/coding/crc16.h>
#include <outpost/parameter/sip.h>
#include <outpost/sip/packet/packet_writer.h>

#include <unittest/harness.h>

class TestPacketWriter : public ::testing::Test
{
public:
    TestPacketWriter()
    {
    }
};

TEST_F(TestPacketWriter, write)
{
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

    // still not finalized
    auto maybeSlice = packetWriter.getSliceIfFinalized();
    ASSERT_FALSE(maybeSlice);

    packetWriter.update();
    auto maybeSlice2 = packetWriter.getSliceIfFinalized();
    ASSERT_TRUE(maybeSlice2);
    outpost::Slice<const uint8_t> bufferToWrite = *maybeSlice2;

    // check length
    ASSERT_EQ(bufferToWrite[0], 0);
    if (outpost::sip::parameter::maxPayloadLength >= 2)
    {
        ASSERT_EQ(bufferToWrite[1], 7);
    }
    // check unitId
    ASSERT_EQ(bufferToWrite[2], 1);
    // check counter
    ASSERT_EQ(bufferToWrite[3], 2);
    // check type
    ASSERT_EQ(bufferToWrite[4], 3);
    // check payload
    if (outpost::sip::parameter::maxPayloadLength >= 2)
    {
        // this test require payload data
        ASSERT_EQ(bufferToWrite[5], 55);
        ASSERT_EQ(bufferToWrite[6], 66);
        return;
    }
    // check CRC
    // compute CRC
    const uint16_t crc = outpost::Crc16Ccitt::calculate(bufferToWrite.skipLast(2));
    EXPECT_EQ(bufferToWrite.last(2)[0], reinterpret_cast<const uint8_t*>(&crc)[1]);
    EXPECT_EQ(bufferToWrite.last(2)[1], reinterpret_cast<const uint8_t*>(&crc)[0]);

    std::size_t const elements = 9 < outpost::sip::parameter::maxPacketLength
                                         ? 9
                                         : outpost::sip::parameter::maxPacketLength;
    ASSERT_EQ(bufferToWrite.getNumberOfElements(), elements);

    if (outpost::sip::parameter::maxPayloadLength < 2)
    {
        // the rest of the test requires the payload data
        return;
    }
    // check finalized is the same
    auto maybeSlice3 = packetWriter.getSliceIfFinalized();
    ASSERT_TRUE(maybeSlice3);
    outpost::Slice<const uint8_t> finalizedBufferToWrite = *maybeSlice3;
    ASSERT_EQ(finalizedBufferToWrite[1], 7);
    ASSERT_EQ(finalizedBufferToWrite[2], 1);
    ASSERT_EQ(finalizedBufferToWrite[3], 2);
    ASSERT_EQ(finalizedBufferToWrite[4], 3);
    ASSERT_EQ(finalizedBufferToWrite[5], 55);
    ASSERT_EQ(finalizedBufferToWrite[6], 66);
    EXPECT_EQ(finalizedBufferToWrite[7], 0xd3);
    EXPECT_EQ(finalizedBufferToWrite[8], 0x62);
    ASSERT_EQ(finalizedBufferToWrite.getNumberOfElements(), elements);
}

TEST_F(TestPacketWriter, maxLength)
{
    std::array<uint8_t, outpost::sip::parameter::maxPacketLength> bufferWriter;
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
    auto maybeSlice = packetWriter.getSliceIfFinalized();
    ASSERT_TRUE(maybeSlice);
    outpost::Slice<const uint8_t> bufferToWrite = *maybeSlice;

    const uint8_t expectedHeaderByte0 = (outpost::sip::parameter::maxPayloadLength + 5) / 256;
    const uint8_t expectedHeaderByte1 = (outpost::sip::parameter::maxPayloadLength + 5) % 256;

    // check length
    EXPECT_EQ(bufferToWrite[0], expectedHeaderByte0);
    EXPECT_EQ(bufferToWrite[1], expectedHeaderByte1);
    // check unitId
    EXPECT_EQ(bufferToWrite[2], 1);
    // check counter
    EXPECT_EQ(bufferToWrite[3], 2);
    // check type
    EXPECT_EQ(bufferToWrite[4], 3);
    // check payload
    for (size_t i = 0; i < outpost::sip::parameter::maxPayloadLength; i++)
    {
        EXPECT_EQ(bufferToWrite[5 + i], i % 256);
    }
    // checking CRC
    const uint16_t crc = outpost::Crc16Ccitt::calculate(bufferToWrite.skipLast(2));
    EXPECT_EQ(bufferToWrite[outpost::sip::parameter::maxPacketLength - 2],
              reinterpret_cast<const uint8_t*>(&crc)[1]);
    EXPECT_EQ(bufferToWrite[outpost::sip::parameter::maxPacketLength - 1],
              reinterpret_cast<const uint8_t*>(&crc)[0]);

    std::size_t elements = outpost::sip::parameter::maxPacketLength;
    ASSERT_EQ(bufferToWrite.getNumberOfElements(), elements);
}

TEST_F(TestPacketWriter, BufferTooSmall)
{
    std::array<uint8_t, outpost::sip::parameter::maxPacketLength - 1> bufferWriter;
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

    auto updateResult = packetWriter.update();
    ASSERT_FALSE(updateResult);
    EXPECT_EQ(updateResult.error(), outpost::sip::OperationResult::lengthErrorTooSmall);

    auto maybeSlice = packetWriter.getSliceIfFinalized();
    ASSERT_FALSE(maybeSlice);
    EXPECT_EQ(maybeSlice.error(), outpost::sip::OperationResult::notFinalized);
}
