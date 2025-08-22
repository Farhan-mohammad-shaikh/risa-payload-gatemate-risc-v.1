/*
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_SPACEWIRE_MULTI_PROTOCOL_HANDLER_H_
#define OUTPOST_HAL_SPACEWIRE_MULTI_PROTOCOL_HANDLER_H_

#include "spacewire.h"

#include <outpost/container/shared_object_pool.h>
#include <outpost/swb/bus_channel.h>
#include <outpost/swb/bus_distributor.h>
#include <outpost/time/clock.h>
#include <outpost/utils/communicator.h>

#include <array>

namespace outpost
{
namespace hal
{
struct MessageID
{
    uint8_t protocol = 0;
    SpaceWire::EndMarker end = SpaceWire::EndMarker::unknown;
};

class SpwPackageFilter : public outpost::swb::MessageFilter<MessageID>
{
public:
    SpwPackageFilter() = default;
    virtual ~SpwPackageFilter() = default;

    void
    setProtocol(uint8_t protocol)
    {
        mProtocol = protocol;
    }

    void
    setAllowPartial(bool partial)
    {
        mPartial = partial;
    }

    void
    setMaxSize(uint32_t maxSize)
    {
        mMaxSize = maxSize;
    }

protected:
    bool
    filter(const MessageID& id, const outpost::Slice<const uint8_t>& m) const override
    {
        return id.protocol == mProtocol && m.getNumberOfElements() <= mMaxSize
               && (mPartial || id.end == SpaceWire::EndMarker::eop);
    }

    uint16_t mProtocol = UINT8_MAX + 1;  // prevents any match before the protocol is set
    bool mPartial = false;
    uint32_t mMaxSize = UINT32_MAX;
};

using SpWChannelInterface = outpost::swb::BusChannel<MessageID>;
using SpWFilteredChannelInterface = outpost::swb::FilteredBusChannel<MessageID, SpwPackageFilter>;
template <size_t size>
using SpWChannel = outpost::swb::BufferedBusChannelWithMemory<size, MessageID, SpwPackageFilter>;
using SpWMessage = outpost::swb::Message<MessageID>;

class SpaceWireMultiProtocolHandlerInterface : public outpost::swb::BusDistributor<MessageID>,
                                               public TimeCodeProvider
{
public:
    SpaceWireMultiProtocolHandlerInterface(
            outpost::utils::Receiver<SpWMessage>& queue,
            uint8_t priority,
            outpost::support::parameter::HeartbeatSource heartbeatSource) :
        outpost::swb::BusDistributor<MessageID>(queue, priority, heartbeatSource)
    {
    }

    virtual ~SpaceWireMultiProtocolHandlerInterface() = default;
    /**
     * Send the contents of buffer
     *
     * @param buffer	data to send
     * @param timeout	maximum time to wait for sending
     *
     * @return 		true  if successful
     * 				false if timeout
     * 						 invalid data
     * 						 any failure in underlying sender
     */
    virtual bool
    send(const outpost::Slice<const uint8_t>& buffer,
         outpost::time::Duration timeout = outpost::time::Duration::zero()) = 0;
};

template <uint32_t maxPackages,          // number of packages that the system can store
          uint32_t maxPacketSize = 4500  // max number of bytes a received package can contain
          >
class SpaceWireMultiProtocolHandler : public SpaceWireMultiProtocolHandlerInterface
{
public:
    SpaceWireMultiProtocolHandler(SpaceWire& spw,
                                  uint8_t priority,
                                  outpost::support::parameter::HeartbeatSource heartbeatSource,
                                  outpost::time::Clock& clock) :
        SpaceWireMultiProtocolHandlerInterface(mSpWHandle, priority, heartbeatSource),
        mSpWHandle(spw),
        mClock(clock){

        };

    virtual ~SpaceWireMultiProtocolHandler() = default;

    /**
     * Send the contents of buffer
     *
     * @param buffer	data to send
     * @param timeout	maximum time to wait for sending
     *
     * @return 		true  if successful
     * 				false if timeout
     * 						 invalid data
     * 						 any failure in underlying sender
     */
    virtual bool
    send(const outpost::Slice<const uint8_t>& buffer,
         outpost::time::Duration timeout = outpost::time::Duration::zero()) override;

    /**
     * Add a listener for timecode
     * @param queue the queue to add
     * @return false if queue == nullptr or all places for Listener are filled
     */
    virtual bool
    addTimeCodeListener(outpost::rtos::Queue<TimeCode>* queue) override;

    /**
     * Starts the underlying thread, call after SpW is opened
     */
    void
    start();

private:
    // As an private inner class so we don't expose an unchecked receive
    class SpaceWireHandle : public outpost::utils::Receiver<SpWMessage>
    {
    public:
        using outpost::utils::Receiver<SpWMessage>::receive;

        static constexpr size_t protocolByteOffset = 1;
        explicit SpaceWireHandle(SpaceWire& spw) : mSpw(spw){};

        virtual ~SpaceWireHandle() = default;

        /**
         * receives a data package
         *
         * @param buffer 	buffer to write the data to
         * @param timeout	max timeout to wait for data
         *
         * @return 0	If no package received in the time or failure in underlying receiver
         * 		   >0   Number of bytes received in package, if larger then
         * buffer.getNumberOfElements(), data has been lost
         */
        outpost::Expected<SpWMessage, outpost::utils::OperationResult>
        receive(outpost::time::Duration timeout) override;

        /**
         * returns the spacewire interface
         */
        inline SpaceWire&
        getSpaceWire();

    private:
        SpaceWire& mSpw;
        outpost::container::SharedBufferPool<maxPacketSize, maxPackages> mPool;
    };

    // use the receiver implementation also to store the spw object for send.
    SpaceWireHandle mSpWHandle;

    outpost::time::Clock& mClock;
};

}  // namespace hal
}  // namespace outpost

#include "space_wire_multi_protocol_handler_impl.h"

#endif /* MODULES_HAL_SRC_OUTPOST_HAL_SPACEWIRE_DISPATCHER_H_ */
