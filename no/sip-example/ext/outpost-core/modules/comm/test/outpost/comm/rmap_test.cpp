/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2018-2021, Jan Malburg
 * Copyright (c) 2020, Felix Passenberg
 * Copyright (c) 2021, Passenberg, Felix Constantin
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Kirstein
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/base/slice.h>
#include <outpost/coding/crc.h>
#include <outpost/comm/rmap/rmap_initiator.h>
#include <outpost/smpc/subscription.h>
#include <outpost/storage/bit_access.h>

#include <unittest/hal/spacewire_stub.h>
#include <unittest/harness.h>
#include <unittest/swb/testing_software_bus.h>
#include <unittest/time/testing_clock.h>

#include <cstdlib>
#include <future>
#include <utility>

using namespace outpost;

namespace outpost
{
namespace comm
{
class TestingRmap
{
public:
    static uint8_t
    getActiveTransactions(RmapInitiator& init)
    {
        return init.mTransactionsList.getNumberOfActiveTransactions();
    }

    static RmapTransaction*
    getFreeTransaction(RmapInitiator& init)
    {
        return init.mTransactionsList.getFreeTransaction();
    }

    static void
    removeTransaction(RmapInitiator& init, uint16_t tid)
    {
        init.mTransactionsList.removeTransaction(tid);
    }

    static RmapTransaction*
    getTransaction(RmapInitiator& init, uint16_t tid)
    {
        return init.mTransactionsList.getTransaction(tid);
    }

    static bool
    isUsedTransaction(RmapInitiator& init, uint16_t tid)
    {
        return init.mTransactionsList.isTransactionIdUsed(tid);
    }

    static bool
    sendPacket(RmapInitiator& init, RmapTransaction* trans)
    {
        return init.sendPacket(trans);
    }

    static bool
    receivePacket(RmapInitiator& init, RmapPacket* pkt, hal::SpWMessage& rxBuffer)
    {
        return init.receivePacket(pkt, rxBuffer);
    }

    static void
    constructPacketHeader(RmapPacket& pckt, const Slice<uint8_t>& buffer)
    {
        // FIXME: Get knowledge about header size and limit buffer to that
        Serialize stream(buffer);
        pckt.constructHeader(stream);
    }

    static void
    initialize(RmapInitiator& init)
    {
        init.mSpW.registerChannel(init.mChannel);
    }

    static void
    step(RmapInitiator& init)
    {
        init.doSingleStep();
    }

    static void
    clear(RmapInitiator& init)
    {
        hal::SpWMessage p;
        while (init.mChannel.receiveMessage(p, time::Duration::zero())
               == swb::OperationResult::success)
        {
        }
    }
};
}  // namespace comm
}  // namespace outpost

using hal::SpaceWire;

using namespace comm;

static unittest::time::TestingClock Clock;

class RmapTest : public testing::Test
{
public:
    static const uint8_t numberOfTargetSpwAddresses = 1;
    static const uint8_t replyAddressLength = 4;
    static uint8_t targetSpwAddress[numberOfTargetSpwAddresses];
    static uint8_t replyAddress[replyAddressLength];
    static const uint8_t targetLogicalAddress = 0xFE;
    static const uint8_t key = 0xFF;
    static const char* targetName;

    RmapTest() :
        mSpaceWire(100),
        mHandler(mSpaceWire, 100, support::parameter::HeartbeatSource::default0, Clock),
        mRmapTarget(targetName, 1, targetLogicalAddress, key),
        mTargetNodes(),
        mRmapInitiator(
                mHandler, &mTargetNodes, 100, 4096, support::parameter::HeartbeatSource::default0),
        mTestingRmap(),
        mTestingBus(mHandler)

    {
        mTestingRmap.initialize(mRmapInitiator);
    }

    virtual void
    SetUp() override
    {
        mSpaceWire.open();
        mSpaceWire.up(time::Duration::zero());
        mRmapTarget.setTargetSpaceWireAddress(asSlice(targetSpwAddress));
        mRmapTarget.setReplyAddress(asSlice(replyAddress));
        mTestingRmap.clear(mRmapInitiator);
    }

    virtual void
    TearDown() override
    {
    }

    void
    handlePackage(Slice<uint8_t> data)
    {
        unittest::hal::SpaceWireStub::Packet packet;
        packet.end = hal::SpaceWire::eop;
        for (auto it = data.begin(); it != data.end(); ++it)
        {
            packet.data.push_back(*it);
        }
        mSpaceWire.mPacketsToReceive.push_back(packet);
        mTestingBus.singleMessage();
    }

    unittest::hal::SpaceWireStub mSpaceWire;
    hal::SpaceWireMultiProtocolHandler<2> mHandler;
    RmapTargetNode mRmapTarget;
    RmapTargetsList mTargetNodes;
    RmapInitiator mRmapInitiator;
    TestingRmap mTestingRmap;
    unittest::swb::TestingSoftwareBus mTestingBus;
};

/**
 * Note: Only working for command with logical addressing!!!
 */
static std::vector<uint8_t>
constructReadReplyPacket(const std::vector<uint8_t>& command,
                         const uint8_t readValue,
                         const uint16_t countRead)
{
    std::vector<uint8_t> ret;
    ret.resize(countRead + 13);

    // data
    for (unsigned int i = 0; i < countRead; i++)
    {
        ret[i + 12] = readValue;
    }
    ret[countRead + 12] = Crc8CcittReversed::calculate(Slice<uint8_t>::unsafe(&ret[12], countRead));

    ret[8] = 0;  // this function does max read replies of uint16_t size
    ret[9] = (countRead & 0xff00) >> 8;
    ret[10] = countRead & 0x00ff;

    // address
    ret[0] = command[4];
    ret[4] = command[0];

    // transaction id
    ret[5] = command[5];
    ret[6] = command[6];

    ret[7] = 0;  // reserved

    ret[1] = rmap::protocolIdentifier;
    ret[2] = command[2] & 0xbf;  // remove command flag
    ret[3] = RmapReplyStatus::commandExecutedSuccessfully;
    ret[11] = Crc8CcittReversed::calculate(Slice<uint8_t>::unsafe(&ret[0], 11));

    return ret;
}

/**
 * Note: Only working for command with logical addressing!!!
 */
static std::vector<uint8_t>
constructReadReplyErrorPacket(const std::vector<uint8_t>& command,
                              const RmapReplyStatus::ErrorStatusCodes error)
{
    std::vector<uint8_t> ret;
    ret.resize(13);

    // data is only its crc
    ret[12] = 0;

    // data error 0 for error
    ret[8] = 0;
    ret[9] = 0;
    ret[10] = 0;

    // address
    ret[0] = command[4];
    ret[4] = command[0];

    // transaction id
    ret[5] = command[5];
    ret[6] = command[6];

    ret[7] = 0;  // reserved

    ret[1] = rmap::protocolIdentifier;
    ret[2] = command[2] & 0xbf;  // remove command flag
    ret[3] = error;
    ret[11] = Crc8CcittReversed::calculate(Slice<uint8_t>::unsafe(&ret[0], 11));

    return ret;
}

/**
 * Note: Only working for command with logical addressing!!!
 */
static std::vector<uint8_t>
constructWriteReplyPacket(const std::vector<uint8_t>& command,
                          const RmapReplyStatus::ErrorStatusCodes error =
                                  RmapReplyStatus::commandExecutedSuccessfully)
{
    std::vector<uint8_t> ret;
    ret.resize(rmap::writeReplyOverhead);

    // address
    ret[0] = command[4];
    ret[4] = command[0];

    // type and result
    ret[1] = rmap::protocolIdentifier;
    ret[2] = command[2] & 0xbf;  // remove command flag
    ret[3] = error;

    // transaction id
    ret[5] = command[5];
    ret[6] = command[6];

    // crc
    ret[7] = Crc8CcittReversed::calculate(Slice<uint8_t>::unsafe(&ret[0], 7));

    return ret;
}

uint8_t RmapTest::targetSpwAddress[numberOfTargetSpwAddresses] = {0};
uint8_t RmapTest::replyAddress[replyAddressLength] = {0, 0, 0, 2};
const char* RmapTest::targetName = "SpWR";

// ----------------------------------------------------------------------------
TEST_F(RmapTest, shouldGetEmptyRmapTargetList)
{
    EXPECT_EQ(0, mTargetNodes.getSize());
    EXPECT_EQ(nullptr, mTargetNodes.getTargetNode(targetName));
}

TEST_F(RmapTest, shouldRegisterRmapTargetInList)
{
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));
}

TEST_F(RmapTest, shouldGetEmptyTransactionsList)
{
    EXPECT_EQ(0, mTestingRmap.getActiveTransactions(mRmapInitiator));
}

TEST_F(RmapTest, removeNonExistentTransaction)
{
    mTestingRmap.getFreeTransaction(mRmapInitiator)->setTransactionID(50);
    EXPECT_EQ(1, mTestingRmap.getActiveTransactions(mRmapInitiator));
    mTestingRmap.removeTransaction(mRmapInitiator, 51);
    EXPECT_EQ(1, mTestingRmap.getActiveTransactions(mRmapInitiator));
}

TEST_F(RmapTest, shouldAddAndRemoveEmptyTransactionInList)
{
    mTestingRmap.getFreeTransaction(mRmapInitiator)->setTransactionID(50);
    EXPECT_EQ(1, mTestingRmap.getActiveTransactions(mRmapInitiator));
    mTestingRmap.removeTransaction(mRmapInitiator, 50);
    EXPECT_EQ(0, mTestingRmap.getActiveTransactions(mRmapInitiator));
}

TEST_F(RmapTest, shouldGetAddedTransactionFromList)
{
    mTestingRmap.getFreeTransaction(mRmapInitiator)->setTransactionID(60);
    EXPECT_EQ(1, mTestingRmap.getActiveTransactions(mRmapInitiator));
    EXPECT_EQ(60, mTestingRmap.getTransaction(mRmapInitiator, 60)->getTransactionID());
}

TEST_F(RmapTest, shouldGetUsedTransactionFromList)
{
    mTestingRmap.getFreeTransaction(mRmapInitiator)->setTransactionID(80);
    EXPECT_EQ(1, mTestingRmap.getActiveTransactions(mRmapInitiator));
    EXPECT_TRUE(mTestingRmap.isUsedTransaction(mRmapInitiator, 80));
}

TEST_F(RmapTest, shouldSetReplyPacketType)
{
    RmapPacket::InstructionField instruction;
    instruction.setPacketType(RmapPacket::InstructionField::replyPacket);
    EXPECT_EQ(RmapPacket::InstructionField::replyPacket, instruction.getPacketType());
}

TEST_F(RmapTest, shouldSetCommandPacketType)
{
    RmapPacket::InstructionField instruction;
    instruction.setPacketType(RmapPacket::InstructionField::commandPacket);
    EXPECT_EQ(RmapPacket::InstructionField::commandPacket, instruction.getPacketType());
}

TEST_F(RmapTest, shouldSetWriteOperationType)
{
    RmapPacket::InstructionField instruction;
    instruction.setOperation(RmapPacket::InstructionField::write);
    EXPECT_EQ(RmapPacket::InstructionField::write, instruction.getOperation());
}

TEST_F(RmapTest, shouldSetReadOperationType)
{
    RmapPacket::InstructionField instruction;
    instruction.setOperation(RmapPacket::InstructionField::read);
    EXPECT_EQ(RmapPacket::InstructionField::read, instruction.getOperation());
}

TEST_F(RmapTest, shouldSetZeroReplyAddressLength)
{
    RmapPacket::InstructionField instruction;

    instruction.setReplyAddressLength(RmapPacket::InstructionField::zeroBytes);
    EXPECT_EQ(RmapPacket::InstructionField::zeroBytes, instruction.getReplyAddressLength());

    instruction.setReplyAddressLength(RmapPacket::InstructionField::fourBytes);
    EXPECT_EQ(RmapPacket::InstructionField::fourBytes, instruction.getReplyAddressLength());

    instruction.setReplyAddressLength(RmapPacket::InstructionField::eightBytes);
    EXPECT_EQ(RmapPacket::InstructionField::eightBytes, instruction.getReplyAddressLength());

    instruction.setReplyAddressLength(RmapPacket::InstructionField::twelveBytes);
    EXPECT_EQ(RmapPacket::InstructionField::twelveBytes, instruction.getReplyAddressLength());
}

TEST_F(RmapTest, shouldBuildVerifyPacketHeaderCRC)
{
    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // *** Constructing command ***
    uint8_t sendBuffer[50] = {0};
    RmapPacket send;
    send.setInitiatorLogicalAddress(rmap::defaultLogicalAddress);
    send.setWrite();
    send.setCommand();
    send.setIncrementFlag(false);
    send.setVerifyFlag(false);
    send.setReplyFlag(false);
    send.setExtendedAddress(rmap::defaultExtendedAddress);
    send.setAddress(0x100);
    send.setDataLength(4);
    send.setTargetInformation(mRmapTarget);
    send.setInitiatorLogicalAddress(0xFE);

    mTestingRmap.constructPacketHeader(send, asSlice(sendBuffer));

    // Acc to RMAP protocol definition, all the SpW target address will be truncated by
    // the router upon receiving the packet at the target node. Thus final packet will be
    // considered without SpW target fields
    uint8_t numberOfTargets = send.getTargetSpaceWireAddress().getNumberOfElements();
    uint8_t calculatedCrc = Crc8CcittReversed::calculate(Slice<uint8_t>::unsafe(
            sendBuffer + numberOfTargets, send.getHeaderLength() - numberOfTargets));

    EXPECT_EQ(calculatedCrc, send.getHeaderCRC());
}

TEST_F(RmapTest, shouldSendWriteCommandPacket)
{
    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // *** Constructing and sending write command ***
    uint8_t buffer[4] = {0x01, 0x02, 0x03, 0x04};
    RmapTransaction transaction;
    RmapPacket* cmd = transaction.getCommandPacket();
    cmd->setInitiatorLogicalAddress(rmap::defaultLogicalAddress);
    cmd->setWrite();
    cmd->setCommand();
    cmd->setIncrementFlag(false);
    cmd->setVerifyFlag(false);
    cmd->setReplyFlag(false);
    transaction.setBlockingMode(false);
    cmd->setExtendedAddress(rmap::defaultExtendedAddress);
    cmd->setAddress(0x100);
    cmd->setDataLength(4);
    cmd->setTargetInformation(mRmapTarget);
    transaction.setInitiatorLogicalAddress(cmd->getInitiatorLogicalAddress());
    transaction.setTimeoutDuration(time::Duration::zero());
    cmd->setData(asSlice(buffer));

    EXPECT_TRUE(mTestingRmap.sendPacket(mRmapInitiator, &transaction));

    size_t expectedSize = 1;

    EXPECT_EQ(expectedSize, mSpaceWire.mSentPackets.size());
    EXPECT_TRUE(mSpaceWire.noUsedTransmitBuffers());
}

TEST_F(RmapTest, shouldReceiveReplyOfWriteCommandPacket)
{
    hal::SpWMessage rxBuffer;
    // *** Constructing and sending write reply packet acc. to RMAP standard ***
    uint8_t reply[20];
    Serialize stream{asSlice(reply)};

    // Construct packet that should be received
    RmapPacket::InstructionField instr;

    instr.setPacketType(RmapPacket::InstructionField::replyPacket);
    instr.setOperation(RmapPacket::InstructionField::write);

    stream.store<uint8_t>(rmap::defaultLogicalAddress);  // Initiator logical address field
    stream.store<uint8_t>(rmap::protocolIdentifier);     // RMAP protocol ID field
    stream.store<uint8_t>(instr.getRaw());               // Instruction field
    stream.store<uint8_t>(0);                            // Status field
    stream.store<uint8_t>(rmap::defaultLogicalAddress);  // Target logical address field
    stream.store<uint16_t>(1);                           // Transaction ID

    uint8_t crc = Crc8CcittReversed::calculate(stream.asSlice());
    stream.store<uint8_t>(crc);  // Header CRC

    handlePackage(asSlice(reply).first(stream.getPosition()));

    RmapPacket receivedPacket;
    EXPECT_TRUE(mTestingRmap.receivePacket(mRmapInitiator, &receivedPacket, rxBuffer));
    EXPECT_TRUE(receivedPacket.isReplyPacket());
    EXPECT_TRUE(receivedPacket.isWrite());

    EXPECT_TRUE(mSpaceWire.mPacketsToReceive.empty());
    EXPECT_TRUE(mSpaceWire.noUsedReceiveBuffers());
}

TEST_F(RmapTest, shouldSendReadCommandPacket)
{
    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // *** Constructing and sending write command ***
    RmapTransaction transaction;
    RmapPacket* cmd = transaction.getCommandPacket();
    cmd->setInitiatorLogicalAddress(rmap::defaultLogicalAddress);
    cmd->setRead();
    cmd->setCommand();
    cmd->setIncrementFlag(false);
    cmd->setVerifyFlag(false);
    cmd->setReplyFlag(true);
    transaction.setBlockingMode(false);
    cmd->setExtendedAddress(rmap::defaultExtendedAddress);
    cmd->setAddress(0x100);
    cmd->setDataLength(4);
    cmd->setTargetInformation(mRmapTarget);
    transaction.setInitiatorLogicalAddress(cmd->getInitiatorLogicalAddress());
    transaction.setTimeoutDuration(time::Duration::zero());

    EXPECT_TRUE(mTestingRmap.sendPacket(mRmapInitiator, &transaction));

    size_t expectedSize = 1;

    EXPECT_EQ(expectedSize, mSpaceWire.mSentPackets.size());
    EXPECT_TRUE(mSpaceWire.noUsedTransmitBuffers());
}

TEST_F(RmapTest, shouldReceiveReplyOfReadCommandPacket)
{
    hal::SpWMessage rxBuffer;
    uint8_t data[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t reply[20];
    Serialize stream{Slice<uint8_t>(reply)};

    // Construct packet that should be received
    RmapPacket::InstructionField instr;

    instr.setPacketType(RmapPacket::InstructionField::replyPacket);
    instr.setOperation(RmapPacket::InstructionField::read);

    stream.store<uint8_t>(rmap::defaultLogicalAddress);  // Initiator logical address field
    stream.store<uint8_t>(rmap::protocolIdentifier);     // RMAP protocol ID field
    stream.store<uint8_t>(instr.getRaw());               // Instruction field
    stream.store<uint8_t>(0);                            // Status field
    stream.store<uint8_t>(rmap::defaultLogicalAddress);  // Target logical address field
    stream.store<uint16_t>(1);                           // Transaction ID
    stream.store<uint8_t>(0);                            // Reserved byte
    stream.store24(sizeof(data));                        // Transaction ID, unsigned

    uint8_t crc = Crc8CcittReversed::calculate(stream.asSlice());
    stream.store<uint8_t>(crc);  // Header CRC

    stream.store(asSlice(data));  // Data bytes to be read

    crc = Crc8CcittReversed::calculate(asSlice(data));

    stream.store<uint8_t>(crc);  // Data CRC

    handlePackage(asSlice(reply).first(stream.getPosition()));

    RmapPacket rxedPacket;
    EXPECT_TRUE(mTestingRmap.receivePacket(mRmapInitiator, &rxedPacket, rxBuffer));
    EXPECT_TRUE(rxedPacket.isReplyPacket());

    for (uint8_t i = 0; i < rxedPacket.getDataLength(); i++)
    {
        EXPECT_EQ(data[i], rxedPacket.getData()[i]);
    }

    EXPECT_TRUE(mSpaceWire.mPacketsToReceive.empty());
    EXPECT_TRUE(mSpaceWire.noUsedReceiveBuffers());
}

TEST_F(RmapTest, shouldSendHigherLevelWriteCommandPacket)
{
    uint8_t dataToSend[4] = {0x01, 0x02, 0x03, 0x04};

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = false;
    options.mReplyMode = false;
    options.mVerifyMode = false;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    EXPECT_TRUE(
            mRmapInitiator.write(targetName, options, address, extaddress, asSlice(dataToSend)));

    size_t expectedSize = 1;

    EXPECT_EQ(expectedSize, mSpaceWire.mSentPackets.size());
    EXPECT_TRUE(mSpaceWire.noUsedTransmitBuffers());

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // we start with the compare at the extended memory address as we can find it nicely
    uint32_t start = 0;
    while (start < packet.data.size() && (packet.data[start] != extaddress))
    {
        start++;
    }

    EXPECT_EQ(start + 10 + sizeof(dataToSend), packet.data.size());
    EXPECT_EQ(packet.data[start + 1], (address >> 24) & 0xff);
    EXPECT_EQ(packet.data[start + 2], (address >> 16) & 0xff);
    EXPECT_EQ(packet.data[start + 3], (address >> 8) & 0xff);
    EXPECT_EQ(packet.data[start + 4], (address) & 0xff);

    uint32_t size = sizeof(dataToSend);

    EXPECT_EQ(packet.data[start + 5], (size >> 16) & 0xff);
    EXPECT_EQ(packet.data[start + 6], (size >> 8) & 0xff);
    EXPECT_EQ(packet.data[start + 7], (size) & 0xff);
    // start + 8 is crc

    EXPECT_EQ(packet.data[start + 9 + 0], dataToSend[0]);
    EXPECT_EQ(packet.data[start + 9 + 1], dataToSend[1]);
    EXPECT_EQ(packet.data[start + 9 + 2], dataToSend[2]);
    EXPECT_EQ(packet.data[start + 9 + 3], dataToSend[3]);
}

TEST_F(RmapTest, testRead)
{
    std::array<uint8_t, 4> readBuffer = {0x00, 0x00, 0x00, 0x00};

    uint8_t readValue = 0xf0;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    auto answer = constructReadReplyPacket(packet.data, readValue, readBuffer.size());

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time to process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, read1.get().getResult());
    }
    else
    {
        FAIL() << "rmap thread did not finish in time";
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    for (unsigned int i = 0; i < sizeof(readBuffer); i++)
    {
        EXPECT_EQ(readBuffer[i], readValue);
    }
}

TEST_F(RmapTest, testAnswerToLong)
{
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    uint8_t readValue = 0xf0;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    // too long answers are invalid
    auto answer = constructReadReplyPacket(packet.data, readValue, sizeof(readBuffer) + 1);

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::invalidReply, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testReadAnswerTooShort)
{
    constexpr uint8_t initialValue = 0x00;
    constexpr uint32_t arraySize = 8;
    uint8_t readBuffer[arraySize] = {initialValue,
                                     initialValue,
                                     initialValue,
                                     initialValue,
                                     initialValue,
                                     initialValue,
                                     initialValue,
                                     initialValue};

    uint8_t readValue = 0xf0;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    const uint32_t answerSize = sizeof(readBuffer) / 2;

    auto answer = constructReadReplyPacket(packet.data, readValue, answerSize);

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        RmapResult result = read1.get();
        EXPECT_EQ(RmapResult::Code::replyTooShort, result.getResult());
        EXPECT_EQ(answerSize, result.getReadBytes());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    // the data we got is still correct
    for (unsigned int i = 0; i < answerSize; i++)
    {
        EXPECT_EQ(readBuffer[i], readValue);
    }

    for (unsigned int i = answerSize; i < arraySize; i++)
    {
        EXPECT_EQ(readBuffer[i], initialValue);
    }
}

TEST_F(RmapTest, testReadFailed)
{
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    mRmapInitiator.resetErrorCounters();

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    RmapReplyStatus::ErrorStatusCodes executionstatus = RmapReplyStatus::generalErrorCode;

    auto answer = constructReadReplyErrorPacket(packet.data, executionstatus);

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        RmapResult result = read1.get();
        EXPECT_EQ(RmapResult::Code::executionFailed, result.getResult());
        EXPECT_EQ(executionstatus, result.getReplyErrorCode());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    auto errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(1u, errorCounters.mOperationFailed);
}

TEST_F(RmapTest, testInvalidParameters)
{
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto ret = mRmapInitiator.read(nullptr, options, address, extaddress, asSlice(readBuffer));
    EXPECT_EQ(RmapResult::Code::invalidParameters, ret.getResult());

    ret = mRmapInitiator.read(targetName, options, address, extaddress, Slice<uint8_t>::empty());
    EXPECT_EQ(RmapResult::Code::invalidParameters, ret.getResult());

    uint8_t tooLong[rmap::bufferSize + 1];
    ret = mRmapInitiator.read(targetName, options, address, extaddress, asSlice(tooLong));
    EXPECT_EQ(RmapResult::Code::invalidParameters, ret.getResult());

    ret = mRmapInitiator.write(nullptr, options, address, extaddress, asSlice(readBuffer));
    EXPECT_EQ(RmapResult::Code::invalidParameters, ret.getResult());

    ret = mRmapInitiator.write(targetName, options, address, extaddress, Slice<uint8_t>::empty());
    EXPECT_EQ(RmapResult::Code::invalidParameters, ret.getResult());
}

TEST_F(RmapTest, testReverseReplyOrder)
{
    static_assert(rmap::numberOfReceiveBuffers >= 2,
                  "Two package switch test requires a receive buffer of at least 2");

    uint8_t readBuffer1[4] = {0x00, 0x00, 0x00, 0x00};
    uint8_t readBuffer2[4] = {0x00, 0x00, 0x00, 0x00};

    uint8_t readValue1 = 0xf0;
    uint8_t readValue2 = 0x0f;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer1));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    auto read2 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(
                targetName, options, address + 4, extaddress, asSlice(readBuffer2));
    });
    read2.wait_for(std::chrono::milliseconds(50));  // give it time to send

    EXPECT_EQ(2u, mSpaceWire.mSentPackets.size());

    // check send package
    auto it = mSpaceWire.mSentPackets.begin();
    auto& packet1 = *it;
    auto& packet2 = *(++it);

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet1.data.size(), 16u);
    EXPECT_EQ(packet2.data.size(), 16u);

    auto answer2 = constructReadReplyPacket(packet2.data, readValue2, sizeof(readBuffer2));

    handlePackage(asSlice(answer2));
    mTestingRmap.step(mRmapInitiator);

    auto status = read2.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, read2.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    auto answer1 = constructReadReplyPacket(packet1.data, readValue1, sizeof(readBuffer1));

    handlePackage(asSlice(answer1));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    for (unsigned int i = 0; i < sizeof(readBuffer1); i++)
    {
        EXPECT_EQ(readBuffer1[i], readValue1);
    }

    for (unsigned int i = 0; i < sizeof(readBuffer2); i++)
    {
        EXPECT_EQ(readBuffer2[i], readValue2);
    }
}

TEST_F(RmapTest, testBufferDoNotLeak)
{
    static_assert(rmap::numberOfReceiveBuffers < rmap::maxConcurrentTransactions,
                  "Test against leakage requires more transaction then receive buffer");

    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    uint8_t readValue = 0xf0;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    for (unsigned int rounds = 0; rounds < rmap::maxConcurrentTransactions; rounds++)
    {
        auto read1 = std::async(std::launch::async, [&]() {
            return mRmapInitiator.read(
                    targetName, options, address, extaddress, asSlice(readBuffer));
        });
        // FIXME: This is sometimes not long enough.
        read1.wait_for(std::chrono::milliseconds(100));  // give it time to send

        // check send package
        ASSERT_GE(mSpaceWire.mSentPackets.size(), 1U)
                << "RMap did not generate a Packet. "
                   "This may be due to a race-condition in the test. "
                   "Re-Run the test some times to see whether this persists, lol";
        auto& packet = *mSpaceWire.mSentPackets.begin();

        // parsing result requires a logical addressing command
        EXPECT_EQ(packet.data.size(), 16u);

        auto answer = constructReadReplyPacket(packet.data, readValue, sizeof(readBuffer));

        handlePackage(asSlice(answer));
        mTestingRmap.step(mRmapInitiator);

        auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

        if (status == std::future_status::ready)
        {
            EXPECT_EQ(RmapResult::Code::success, read1.get().getResult());
        }
        else
        {
            EXPECT_TRUE(false);
            exit(-1);  // no other way to stop the threads, unit tests will still fail.
        }

        for (unsigned int i = 0; i < sizeof(readBuffer); i++)
        {
            EXPECT_EQ(readBuffer[i], readValue);
        }

        mSpaceWire.mSentPackets.clear();

        readValue++;  // change the value for more testing
    }
}

TEST_F(RmapTest, testInvaldRepliesAndErrorCounters)
{
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    uint8_t readValueInCor1 = 0x10;
    uint8_t readValueInCor2 = 0x20;
    uint8_t readValueInCor3 = 0x30;
    uint8_t readValueInCor4 = 0x40;
    uint8_t readValueInCor5 = 0x50;
    uint8_t readValue = 0xf0;

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    mRmapInitiator.resetErrorCounters();

    auto errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(0u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(0u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(0u, errorCounters.mInvalidSize);

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(targetName, options, address, extaddress, asSlice(readBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    // first wrong header crc
    auto answer = constructReadReplyPacket(packet.data, readValueInCor1, sizeof(readBuffer));
    answer[11]++;

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(0u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(1u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(0u, errorCounters.mInvalidSize);

    // wrong data crc
    answer = constructReadReplyPacket(packet.data, readValueInCor2, sizeof(readBuffer));
    (*answer.rbegin())++;

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(0u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(0u, errorCounters.mInvalidSize);

    // partial package
    answer = constructReadReplyPacket(packet.data, readValueInCor3, sizeof(readBuffer));

    handlePackage(asSlice(answer).skipLast(1));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(0u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(1u, errorCounters.mInvalidSize);

    // partial package very small
    answer = constructReadReplyPacket(packet.data, readValueInCor3, sizeof(readBuffer));

    handlePackage(asSlice(answer).first(rmap::minimumReplySize - 1));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(0u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(2u, errorCounters.mInvalidSize);

    // incorrect type
    std::vector<uint8_t> tmp = packet.data;
    tmp[2] ^= 0x04;  // change: increment mode

    answer = constructReadReplyPacket(tmp, readValueInCor4, sizeof(readBuffer));

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(1u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(0u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(2u, errorCounters.mInvalidSize);

    // incorrect id
    tmp = packet.data;
    tmp[6] += 1;
    answer = constructReadReplyPacket(tmp, readValueInCor5, sizeof(readBuffer));

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_TRUE(false);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(1u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(1u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(2u, errorCounters.mInvalidSize);

    // correct version
    answer = constructReadReplyPacket(packet.data, readValue, sizeof(readBuffer));

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }

    for (unsigned int i = 0; i < sizeof(readBuffer); i++)
    {
        EXPECT_EQ(readBuffer[i], readValue);
    }

    errorCounters = mRmapInitiator.getErrorCounters();
    EXPECT_EQ(1u, errorCounters.mIncorrectOperation);
    EXPECT_EQ(0u, errorCounters.mOperationFailed);
    EXPECT_EQ(2u, errorCounters.mPackageCrcError);
    EXPECT_EQ(0u, errorCounters.mSpacewireFailure);
    EXPECT_EQ(1u, errorCounters.mUnknownTransactionID);
    EXPECT_EQ(2u, errorCounters.mInvalidSize);
}

TEST_F(RmapTest, testTimeoutRead)
{
    time::Duration waitTime = time::Seconds(1);
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(
                targetName, options, address, extaddress, asSlice(readBuffer), waitTime);
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), 16u);

    auto status = read1.wait_for(
            std::chrono::milliseconds(waitTime.milliseconds() * 2));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::timeout, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testWrite)
{
    uint8_t writeBuffer[4] = {0x01, 0x02, 0x03, 0x04};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto write = std::async(std::launch::async, [&]() {
        return mRmapInitiator.write(targetName, options, address, extaddress, asSlice(writeBuffer));
    });
    write.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), rmap::writeCommandOverhead + sizeof(writeBuffer));

    for (unsigned int i = 0; i < sizeof(writeBuffer); i++)
    {
        // data crc is behind data
        EXPECT_EQ(packet.data[(rmap::writeCommandOverhead - 1) + i], writeBuffer[i]);
    }

    auto answer = constructWriteReplyPacket(packet.data);

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = write.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, write.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testWriteFailed)
{
    uint8_t writeBuffer[4] = {0x01, 0x02, 0x03, 0x04};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto write = std::async(std::launch::async, [&]() {
        return mRmapInitiator.write(targetName, options, address, extaddress, asSlice(writeBuffer));
    });
    write.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), rmap::writeCommandOverhead + sizeof(writeBuffer));

    for (unsigned int i = 0; i < sizeof(writeBuffer); i++)
    {
        // data crc is behind data
        EXPECT_EQ(packet.data[(rmap::writeCommandOverhead - 1) + i], writeBuffer[i]);
    }

    RmapReplyStatus::ErrorStatusCodes error = RmapReplyStatus::invalidDataCrc;

    auto answer = constructWriteReplyPacket(packet.data, error);

    handlePackage(asSlice(answer));
    mTestingRmap.step(mRmapInitiator);

    auto status = write.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        auto r = write.get();
        EXPECT_EQ(RmapResult::Code::executionFailed, r.getResult());
        EXPECT_EQ(error, r.getReplyErrorCode());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testTimeoutWrite)
{
    time::Duration waitTime = time::Seconds(1);
    uint8_t writeBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.write(
                targetName, options, address, extaddress, asSlice(writeBuffer), waitTime);
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), rmap::writeCommandOverhead + sizeof(writeBuffer));

    auto status = read1.wait_for(
            std::chrono::milliseconds(waitTime.milliseconds() * 2));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::timeout, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testNoReplyWrite)
{
    uint8_t writeBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = false;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    auto read1 = std::async(std::launch::async, [&]() {
        return mRmapInitiator.write(targetName, options, address, extaddress, asSlice(writeBuffer));
    });
    read1.wait_for(std::chrono::milliseconds(50));  // give it time to send

    // check send package
    auto& packet = *mSpaceWire.mSentPackets.begin();

    // parsing result requires a logical addressing command
    EXPECT_EQ(packet.data.size(), rmap::writeCommandOverhead + sizeof(writeBuffer));

    auto status = read1.wait_for(std::chrono::milliseconds(50));  // give it time process reply

    if (status == std::future_status::ready)
    {
        EXPECT_EQ(RmapResult::Code::success, read1.get().getResult());
    }
    else
    {
        EXPECT_TRUE(false);
        exit(-1);  // no other way to stop the threads, unit tests will still fail.
    }
}

TEST_F(RmapTest, testTooManyTransactions)
{
    time::Duration waitTime = time::Seconds(2);
    uint8_t readBuffer[4] = {0x00, 0x00, 0x00, 0x00};

    // for easier parsing of send command
    mRmapTarget.setReplyAddress(Slice<uint8_t>::empty());
    mRmapTarget.setTargetSpaceWireAddress(Slice<uint8_t>::empty());

    // Register RMAP target
    EXPECT_TRUE(mTargetNodes.addTargetNode(&mRmapTarget));
    EXPECT_EQ(1, mTargetNodes.getSize());
    EXPECT_EQ(&mRmapTarget, mTargetNodes.getTargetNode(targetName));

    // Send RMAP command to read from the target
    RMapOptions options;
    options.mIncrementMode = true;
    options.mReplyMode = true;
    options.mVerifyMode = true;

    static const uint8_t extaddress = 0x7e;
    static const uint32_t address = 0x1000;

    // one moved out to get the type
    auto f = std::async(std::launch::async, [&]() {
        return mRmapInitiator.read(
                targetName, options, address, extaddress, asSlice(readBuffer), waitTime);
    });

    decltype(f) futurs[rmap::maxConcurrentTransactions + 1];
    futurs[rmap::maxConcurrentTransactions] = std::move(f);
    for (unsigned int i = 0; i < rmap::maxConcurrentTransactions; i++)
    {
        futurs[i] = std::async(std::launch::async, [&]() {
            return mRmapInitiator.read(
                    targetName, options, address, extaddress, asSlice(readBuffer), waitTime);
        });
    }

    // for one there will be no transaction but as they are parallel starting we don't know which
    uint32_t timeOuted = 0;
    uint32_t noFreeTransaction = 0;

    // clear the running threads
    for (unsigned int i = 0; i < rmap::maxConcurrentTransactions + 1; i++)
    {
        auto status = futurs[i].wait_for(std::chrono::milliseconds(waitTime.milliseconds() * 2));
        if (status == std::future_status::ready)
        {
            auto value = futurs[i].get().getResult();
            if (value == RmapResult::Code::timeout)
            {
                timeOuted++;
            }
            else if (value == RmapResult::Code::noFreeTransactions)
            {
                noFreeTransaction++;
            }
            else
            {
                // Unexpected result
                EXPECT_EQ(value, RmapResult::Code::timeout);
                exit(-1);  // no other way to stop the threads, unit tests will still fail.
            }
        }
        else
        {
            EXPECT_EQ(status, std::future_status::ready);
            exit(-1);  // no other way to stop the threads, unit tests will still fail.
        }
    }

    EXPECT_EQ(1u, noFreeTransaction);
    EXPECT_EQ(rmap::maxConcurrentTransactions, timeOuted);
}

TEST(RMapPacket, defaultConstructor)
{
    RmapPacket packet{};
    EXPECT_EQ(packet.getDataLength(), 0U);
}

TEST(RMapPacket, constructorOneWayPacket)
{
    RmapPacket packet{1, 0, 2, 0xDEAD, 4};
    EXPECT_EQ(packet.getDataLength(), 4U);
}

TEST(RMapPacket, constructorReply)
{
    std::array<uint8_t, 4> targets = {1, 2, 3, 4};
    std::array<uint8_t, 4> replyAddress = {4, 3, 2, 1};
    RmapPacket packet{asSlice(targets),
                      1,
                      RmapPacket::InstructionField::ReplyAddressLength::fourBytes,
                      0,
                      replyAddress.begin(),
                      2,
                      0xDEAD,
                      2};
    EXPECT_EQ(packet.getDataLength(), 2U);
}
