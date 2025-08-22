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

#ifndef OUTPOST_TRANSPORT_FRAME_DECODER_H
#define OUTPOST_TRANSPORT_FRAME_DECODER_H

#include <outpost/base/slice.h>
#include <outpost/transport/operation_result.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace transport
{
/**
 * Chunkwise Frame Decoder
 *
 */
class FrameDecoder
{
public:
    virtual ~FrameDecoder() = default;

    /**
     * Decode a frame into the destination buffer.
     *
     * \param[in] input
     *      Encoded frame as received from the Device.
     *
     *      The bounded array in the frame points into the array given
     *      through this parameter.
     * \param[in] writeBuffer
     *      Buffer in which the decoded bytes are placed.
     *
     * \return Expected
     * \retval true: Slice
     *      Input successfully decoded. Slice is bounded to valid data.
     * \retval false: decodingError
     *      Decoding Error happened during decoding.
     *      No valid buffer header is present in the buffer parameter.
     *      The data in \p writeBuffer might be altered.
     */
    virtual outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    decode(outpost::Slice<const uint8_t> const& input,
           outpost::Slice<uint8_t> const& writeBuffer) = 0;
};

}  // namespace transport
}  // namespace outpost
#endif
