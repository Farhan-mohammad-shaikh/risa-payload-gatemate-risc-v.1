/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2020, Felix Passenberg
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_HAL_SPACEWIRE_STUB_H
#define UNITTEST_HAL_SPACEWIRE_STUB_H

#include <outpost/hal/spacewire.h>

#include <list>
#include <map>
#include <memory>
#include <vector>

namespace unittest
{
namespace hal
{
/**
 * SpaceWire interface stub.
 *
 * Provides direct access to the send- and receive queue. The blocking mode
 * is ignored, every action happens instantly.
 *
 * To use this class fill the mPacketsToReceive member variable which should
 * be received during the test. In the mSentPackets all sent packets can be
 * collected after the test.
 *
 * \author  Fabian Greif
 */
class SpaceWireStub : public outpost::hal::SpaceWire
{
public:
    struct Packet
    {
        std::vector<uint8_t> data;
        EndMarker end;
    };

    /**
     * \param  maximumLength
     *      Maximum length of single SpaceWire packet.
     */
    explicit SpaceWireStub(size_t maximumLength);

    virtual ~SpaceWireStub();

    virtual size_t
    getMaximumPacketLength() const override;

    bool
    open() override;

    void
    close() override;

    bool
    up(outpost::time::Duration timeout) override;

    void
    down(outpost::time::Duration timeout) override;

    bool
    isUp() override;

    outpost::Expected<outpost::hal::SpaceWire::TransmitBuffer*,
                      outpost::hal::SpaceWire::OperationResult>
    requestBuffer(outpost::time::Duration timeout) override;

    OperationResult
    send(TransmitBuffer* buffer, outpost::time::Duration timeout) override;

    outpost::Expected<outpost::hal::SpaceWire::ReceiveBuffer,
                      outpost::hal::SpaceWire::OperationResult>
    receive(outpost::time::Duration timeout) override;

    void
    releaseBuffer(const ReceiveBuffer& buffer) override;

    // Ignored in this implementation.
    void
    flushReceiveBuffer() override;

    /**
     * Check that no transmit buffers are currently used.
     *
     * \retval  true    All transmit buffers have been returned by sending.
     * \retval  false   One or more transmit buffers are still in use by
     *                  the application.
     */
    bool
    noUsedTransmitBuffers() const
    {
        return mTransmitBuffers.empty();
    }

    /**
     * Check that no receive buffer are in used.
     */
    bool
    noUsedReceiveBuffers() const
    {
        return mReceiveBuffers.empty();
    }

    bool
    addTimeCodeListener(outpost::rtos::Queue<outpost::hal::TimeCode>* queue) override
    {
        return mTCD.addListener(queue);
    }

    /**
     * Filled with all packets sent through SpaceWire interface.
     */
    std::list<Packet> mSentPackets;

    /**
     * Packets which can be received through the receive() function.
     *
     * Fill with data before starting the operation.
     */
    std::list<Packet> mPacketsToReceive;

    /*
     * Simulates an SpWInterrupt
     */
    void
    triggerSpWInterrupt(void);

private:
    struct TransmitBufferEntry
    {
        explicit inline TransmitBufferEntry(size_t maximumLength) :
            buffer(maximumLength, 0), header(outpost::asSlice(buffer))
        {
        }

        std::vector<uint8_t> buffer;
        TransmitBuffer header;
    };

    struct ReceiveBufferEntry
    {
        ReceiveBufferEntry(std::vector<uint8_t>&& input, EndMarker end) :
            buffer(std::move(input)), header(outpost::asSlice(buffer), end)
        {
        }

        std::vector<uint8_t> buffer;
        ReceiveBuffer header;
    };

    const size_t mMaximumLength;
    bool mOpen;
    bool mUp;

    outpost::hal::TimeCodeDispatcher<1> mTCD;

    /**
     * Mapping between the transmit buffer given to the user and its storage
     * location.
     */
    std::map<TransmitBuffer*, std::unique_ptr<TransmitBufferEntry>> mTransmitBuffers;
    std::map<const uint8_t*, std::unique_ptr<ReceiveBufferEntry>> mReceiveBuffers;
    outpost::rtos::Mutex mOperationLock;
};

}  // namespace hal
}  // namespace unittest

#endif
