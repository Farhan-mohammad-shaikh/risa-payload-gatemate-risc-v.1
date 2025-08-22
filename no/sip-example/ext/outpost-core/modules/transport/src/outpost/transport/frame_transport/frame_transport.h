/*
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_FRAME_TRANSPORT_H
#define OUTPOST_TRANSPORT_FRAME_TRANSPORT_H

#include <outpost/base/slice.h>
#include <outpost/time/timeout.h>
#include <outpost/transport/operation_result.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace transport
{
/**
 * Transmitting a frame.
 *
 */
class FrameTransportTx
{
public:
    virtual ~FrameTransportTx() = default;

    /**
     * Transmit input bytes as a frame.
     *
     * \ param inputBytes
     *      Input bytes to be sent
     *
     * \return Expected
     * \retval true size_t
     *      Success Number of Bytes transmitted
     * \retval false OperationResult
     *      Failed
     * \retval other
     *      forwarded OpertionResult from \c FrameEncoder.
     */
    virtual outpost::Expected<size_t, OperationResult>
    transmit(const outpost::Slice<const uint8_t>& inputBytes) = 0;
};

/**
 * Receiving of a frame.
 *
 */
class FrameTransportRx
{
public:
    virtual ~FrameTransportRx() = default;

    /**
     * Receive a frame.
     *
     * This is a synchronous operation, the function will only return after receiving
     * a frame or reaching the timeout.
     *
     * \param[in/out] outputBuffer
     *      Destination buffer for received frame
     * \param timeout
     *      max time for the operation.
     *
     * \return Expected
     * \retval success
     *      A slice containing the received data bound to the outputBuffer provided.
     * \retval false decodeError
     *      FrameEncoder decode error
     * \retval false bufferError
     *      Buffer size error
     * \retval false timeOut
     *      Timeout occurred
     * \retval other
     *      forwarded OpertionResult from \c FrameEncoder.
     */
    virtual outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    receive(outpost::Slice<uint8_t> const& outputBuffer, outpost::time::Duration timeout) = 0;
};

class FrameTransport : public virtual FrameTransportTx, public virtual FrameTransportRx
{
public:
    virtual ~FrameTransport() = default;
};

}  // namespace transport
}  // namespace outpost
#endif
