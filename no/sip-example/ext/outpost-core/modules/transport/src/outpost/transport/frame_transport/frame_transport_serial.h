/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, cold_ei
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_FRAME_TRANSPORT_SERIAL_H
#define OUTPOST_TRANSPORT_FRAME_TRANSPORT_SERIAL_H

#include <outpost/coding/hdlc.h>
#include <outpost/hal/serial.h>
#include <outpost/rtos/mutex.h>
#include <outpost/rtos/thread.h>
#include <outpost/storage/serialize.h>
#include <outpost/time/clock.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder.h>
#include <outpost/transport/frame_coder/frame_encoder.h>
#include <outpost/transport/frame_transport/frame_transport.h>

namespace outpost
{
namespace transport
{
/**
 * Transmitting a frame through a serial connection.
 */
class FrameTransportTxSerial : public virtual FrameTransportTx
{
public:
    /**
     * Creates the frame transport tx serial
     *
     * \param serialTx
     *      Serial interface to transmit data
     * \param frameEncoder
     *      Frame encoder (HDLC, COBS, ...)
     * \param transmitBuffer
     *      Buffer to hold encoded result during transmit
     *      \note Underlying array should be at least the size of the worse case encoded frame
     */
    FrameTransportTxSerial(outpost::hal::SerialTx& serialTx,
                           outpost::transport::FrameEncoder& frameEncoder,
                           outpost::Slice<uint8_t> transmitBuffer);

    FrameTransportTxSerial(const FrameTransportTxSerial&) = delete;

    FrameTransportTxSerial&
    operator=(const FrameTransportTxSerial&) = delete;

    virtual ~FrameTransportTxSerial();

    /**
     * Transmit a frame.
     *
     * Concurrent transmits will wait on a mutex to prevent interference.
     *
     * \param inputBytes
     *      Frame to be send
     *
     * \return Total number of transmitted bytes (Including framing/encoding overhead)
     *         See also base class \c FrameTransport
     *
     */
    outpost::Expected<size_t, OperationResult>
    transmit(const outpost::Slice<const uint8_t>& inputBytes) override;

private:
    outpost::hal::SerialTx& mSerialTx;
    outpost::transport::FrameEncoder& mFrameEncoder;
    outpost::Slice<uint8_t> mTransmitBuffer;

    outpost::rtos::Mutex mTxMutex;
};

class FrameTransportRxSerial : public virtual FrameTransportRx
{
public:
    /**
     * Creates the frame transport rx serial
     *
     * \param clock
     *      Clock for timeout and sleep
     * \param serialRx
     *      Serial interface to receive data
     * \param frameDecoder
     *      Buffered frame decoder (HDLC, COBS, ...)
     * \param clearOnTimeout
     *      Will clear the decode buffer on timeout during receive
     * \param serialReadTimeoutIn
     *      Time out for serial read
     * \param waitForDataSleep
     *      Sleep time for wait new data
     */
    FrameTransportRxSerial(
            const outpost::time::Clock& clock,
            outpost::hal::SerialRx& serialRx,
            outpost::transport::BufferedFrameDecoder& frameDecoder,
            bool clearOnTimeout = true,
            outpost::time::Duration serialReadTimeout = outpost::time::Milliseconds(10),
            outpost::time::Duration waitForDataSleepTime = outpost::time::Milliseconds(10));

    virtual ~FrameTransportRxSerial();

    /**
     * Receive a frame.
     *
     * The function will only return after receiving
     * a frame or reaching a timeout.
     *
     * The data in the decoder buffer will be cleared if mClearOnTimeout is true.
     *
     * \param outputBuffer
     *      Destination buffer for received frame
     * \param timeout
     *      Timeout
     *
     * \return see base class \c FrameTransport
     *
     */
    outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    receive(outpost::Slice<uint8_t> const& outputBuffer, outpost::time::Duration timeout) override;

private:
    const outpost::time::Clock& mClock;
    outpost::hal::SerialRx& mSerialRx;
    outpost::transport::BufferedFrameDecoder& mFrameDecoder;
    bool mClearOnTimeout;
    const outpost::time::Duration mSerialReadTimeout;
    const outpost::time::Duration mWaitForDataSleepTime;
};

class FrameTransportSerial : public FrameTransportTxSerial,
                             public FrameTransportRxSerial,
                             public FrameTransport
{
public:
    FrameTransportSerial(
            const outpost::time::Clock& clock,
            outpost::hal::Serial& serial,
            outpost::transport::FrameEncoder& frameEncoder,
            outpost::Slice<uint8_t> transmitBuffer,
            outpost::transport::BufferedFrameDecoder& frameDecoder,
            bool clearOnTimeout = true,
            outpost::time::Duration serialReadTimeout = outpost::time::Milliseconds(10),
            outpost::time::Duration waitForDataSleepTime = outpost::time::Milliseconds(10));

    using FrameTransportRxSerial::receive;
    using FrameTransportTxSerial::transmit;

    virtual ~FrameTransportSerial() = default;
};

}  // namespace transport
}  // namespace outpost

#endif
