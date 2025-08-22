/*
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SPW_POSIX_STARDUNDEE_DEVICE_H_
#define OUTPOST_SPW_POSIX_STARDUNDEE_DEVICE_H_

#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <device_list.hpp>
#pragma GCC diagnostic pop

#include <outpost/hal/spacewire.h>
#include <outpost/hal/timecode.h>
#include <outpost/rtos/queue.h>

namespace outpost
{
namespace spw_posix
{
enum class DeviceType : uint8_t
{
    BRICK_MK3 = 19,
    BRICK_MK4 = 32,
};

enum class LinkSpeed : uint8_t
{
    MBPS_10 = 0,
    MBPS_20 = 1,
    MBPS_50 = 2,
    MBPS_100 = 3,
    MBPS_200 = 4
};

class StardundeeSpacewire : public outpost::hal::SpaceWire
{
public:
    StardundeeSpacewire(const char* serialNumber, DeviceType type);
    virtual ~StardundeeSpacewire();

    inline void
    setPort(uint8_t p)
    {
        mPort = p;
    }
    inline uint8_t
    getPort()
    {
        return mPort;
    }

    inline void
    setChannel(uint8_t c)
    {
        mChannelIndex = c;
    }
    inline uint8_t
    getChannel()
    {
        return mChannelIndex;
    }

    inline void
    setLinkSpeed(LinkSpeed ls)
    {
        mLinkSpeed = ls;
    }
    inline LinkSpeed
    getLinkSpeed()
    {
        return mLinkSpeed;
    }

    virtual bool
    isValid();

    virtual bool
    configure() = 0;

    virtual bool
    open() override;
    virtual void
    close() override = 0;

    virtual inline size_t
    getMaximumPacketLength() const override
    {
        return maximumPacketLength;
    }

    virtual bool
    up(outpost::time::Duration timeout) override = 0;

    virtual void
    down(outpost::time::Duration timeout) override = 0;

    virtual bool
    isUp() override = 0;

    virtual Result::Type
    requestBuffer(TransmitBuffer*& buffer, outpost::time::Duration timeout) override;

    virtual Result::Type
    send(TransmitBuffer* buffer, outpost::time::Duration timeout) override;

    virtual Result::Type
    receive(ReceiveBuffer& buffer, outpost::time::Duration timeout) override;

    virtual void
    releaseBuffer(const ReceiveBuffer& buffer) override;

    virtual void
    flushReceiveBuffer() override = 0;

    virtual bool
    addTimeCodeListener(outpost::rtos::Queue<outpost::hal::TimeCode>*) override;

protected:
    static constexpr size_t numberOfReceiveBuffers = 10;
    static constexpr size_t numberOfTransmitBuffers = 10;

    // Must be divisible by four.
    static constexpr size_t maximumPacketLength = 4500;
    static constexpr size_t minimumPacketLength = 4;

    static constexpr size_t dataBodyOffset = 0;

    void
    initializeBuffers();

    bool
    getTransmitBuffer(TransmitBuffer*& buffer);
    void
    releaseTransmitBuffer(uint8_t* dataBodyPointer);

    bool
    getReceiveBuffer(uint8_t*& buffer);

    outpost::rtos::Mutex mTransmissionMutex;
    outpost::rtos::Mutex mReceptionMutex;
    outpost::rtos::BinarySemaphore mTransmitEvent;
    outpost::rtos::BinarySemaphore mReceiveBufferReleaseEvent;

    using TransmitBufferData = uint8_t[maximumPacketLength];
    TransmitBufferData mTransmitBufferData[numberOfTransmitBuffers];
    TransmitBuffer mTransmitBuffer[numberOfTransmitBuffers];
    bool mTransmitBufferUsed[numberOfTransmitBuffers];

    LinkSpeed mLinkSpeed;
    uint8_t mPort;
    uint8_t mChannelIndex;

    stardundee::com::starsystem::deviceconfig::DeviceConfiguration* mConfiguration;
    stardundee::com::starsystem::channels::Channel mChannel;

    uint8_t mReceiveBufferData[numberOfReceiveBuffers][maximumPacketLength];
    bool mReceiveBufferUsed[numberOfReceiveBuffers];

    stardundee::com::starsystem::general::Device* mDevice;
    const char* mSerialNumber;
    DeviceType mType;
};

}  // namespace spw_posix
}  // namespace outpost

#endif /* EXT_OUTPOST_PLATFORM_POSIX_SRC_OUTPOST_SPW_POSIX_STARDUNDEE_DEVICE_H_ \
        */
