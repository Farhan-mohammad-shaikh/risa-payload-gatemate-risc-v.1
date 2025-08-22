/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Tepe, Alexander
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "frame_transport_serial.h"

#include <outpost/rtos/mutex_guard.h>

namespace outpost
{
namespace transport
{
FrameTransportTxSerial::FrameTransportTxSerial(outpost::hal::SerialTx& serialTx,
                                               FrameEncoder& frameEncoder,
                                               outpost::Slice<uint8_t> transmitBuffer) :
    mSerialTx(serialTx), mFrameEncoder(frameEncoder), mTransmitBuffer(transmitBuffer)
{
}

FrameTransportTxSerial::~FrameTransportTxSerial()
{
}

outpost::Expected<size_t, OperationResult>
FrameTransportTxSerial::transmit(const outpost::Slice<const uint8_t>& inputBytes)
{
    outpost::rtos::MutexGuard lock(mTxMutex);

    auto const maybeEncodedBytes = mFrameEncoder.encode(inputBytes, mTransmitBuffer);
    if (!maybeEncodedBytes)
    {
        return outpost::unexpected(maybeEncodedBytes.error());
    }
    const auto& encodedBytes = *maybeEncodedBytes;

    size_t bytesCounter = 0;
    do
    {
        auto const ret = mSerialTx.write(encodedBytes.skipFirst(bytesCounter));
        if (!ret)
        {
            return outpost::unexpected(OperationResult::streamStopped);
        }
        bytesCounter += *ret;
    } while (bytesCounter < encodedBytes.getNumberOfElements());
    return bytesCounter;
}

FrameTransportRxSerial::FrameTransportRxSerial(
        const outpost::time::Clock& clock,
        outpost::hal::SerialRx& serialRx,
        outpost::transport::BufferedFrameDecoder& frameDecoder,
        bool clearOnTimeout,
        outpost::time::Duration serialReadTimeout,
        outpost::time::Duration waitForDataSleepTime) :
    mClock(clock),
    mSerialRx(serialRx),
    mFrameDecoder(frameDecoder),
    mClearOnTimeout(clearOnTimeout),
    mSerialReadTimeout(serialReadTimeout),
    mWaitForDataSleepTime(waitForDataSleepTime)
{
}

FrameTransportRxSerial::~FrameTransportRxSerial()
{
}

outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
FrameTransportRxSerial::receive(outpost::Slice<uint8_t> const& outputBuffer,
                                const outpost::time::Duration timeout)
{
    OperationResult status = OperationResult::timeout;
    const auto startTime = mClock.now();
    const bool isNonBlockingCall = timeout == outpost::time::Duration::zero();

    while (true)
    {
        const auto timeElapsed = mClock.now() - startTime;
        const auto timeRemaining = timeout - timeElapsed;

        if (!isNonBlockingCall && timeRemaining <= outpost::time::Duration::zero())
        {
            status = OperationResult::timeout;
            break;
        }

        uint8_t temp[1];
        outpost::Slice<uint8_t> tempSlice = outpost::asSlice(temp);
        const auto boundedTimeout = std::min(timeRemaining, mSerialReadTimeout);
        const auto bytesRead = mSerialRx.read(tempSlice, boundedTimeout);

        if (!bytesRead)
        {
            status = OperationResult::streamStopped;
            break;
        }
        else if (*bytesRead == 0)
        {
            // nothing received (serial timed out)
            if (isNonBlockingCall)
            {
                return unexpected(OperationResult::timeout);
            }
            outpost::rtos::Thread::sleep(mWaitForDataSleepTime);
        }
        else
        {
            auto const result = mFrameDecoder.bufferedDecode(temp[0], outputBuffer);

            if (result)
            {
                // frame was decoded successfully
                return *result;
            }
            else if (result.error() == OperationResult::notComplete)
            {
                /* no error, just need more bytes to decode */
            }
            else
            {
                status = result.error();
                break;
            }
        }
    }

    if (status == OperationResult::timeout)
    {
        if (mClearOnTimeout)
        {
            mFrameDecoder.reset();
        }
        return outpost::unexpected(status);
    }
    else
    {
        mFrameDecoder.reset();
        return outpost::unexpected(status);
    }
}

FrameTransportSerial::FrameTransportSerial(const outpost::time::Clock& clock,
                                           outpost::hal::Serial& serial,
                                           outpost::transport::FrameEncoder& frameEncoder,
                                           outpost::Slice<uint8_t> transmitBuffer,
                                           outpost::transport::BufferedFrameDecoder& frameDecoder,
                                           bool clearOnTimeout,
                                           outpost::time::Duration serialReadTimeout,
                                           outpost::time::Duration waitForDataSleepTime) :
    FrameTransportSerial::FrameTransportTxSerial(serial, frameEncoder, transmitBuffer),
    FrameTransportSerial::FrameTransportRxSerial(
            clock, serial, frameDecoder, clearOnTimeout, serialReadTimeout, waitForDataSleepTime)
{
}

}  // namespace transport
}  // namespace outpost
