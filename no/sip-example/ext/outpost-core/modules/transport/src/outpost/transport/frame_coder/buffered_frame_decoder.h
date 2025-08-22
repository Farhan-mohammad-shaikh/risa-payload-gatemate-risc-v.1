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

#ifndef OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_H
#define OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_H

#include <outpost/base/slice.h>
#include <outpost/transport/operation_result.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace transport
{
/**
 * Frame decoder working with byte streams
 */
class BufferedFrameDecoder
{
public:
    virtual ~BufferedFrameDecoder() = default;

    /**
     * Decode a frame bytewise into the destination buffer.
     *
     * Input data is internally buffered and cleared when a frame has been decoded.
     *
     * \param[in] input
     *      Byte as received from the device.
     * \param[in] writeBuffer
     *      Buffer in which the decoded bytes are placed.
     *
     * \return Expected
     * \retval true: Slice
     *      Input successfully decoded. Slice is bounded to valid data.
     * \retval false: notComplete
     *      Not enough data has been provided to decode a frame
     * \retval false: bufferError
     *      The internal buffer would overflow with the additional input.
     * \retval false: decodingError
     *      Decoding Error happened during decoding.
     *      No valid buffer header is present in the buffer parameter.
     *      The data in \p writeBuffer might be altered.
     */
    virtual outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    bufferedDecode(const uint8_t input, const outpost::Slice<uint8_t>& writeBuffer) = 0;

    /**
     * Resets any internal state and clears the buffer.
     */
    virtual void
    reset() = 0;
};

}  // namespace transport
}  // namespace outpost

#endif  // OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_H
