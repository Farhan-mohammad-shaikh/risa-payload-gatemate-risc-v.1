/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_TRANSPORT_FRAME_ENCODER_H
#define OUTPOST_TRANSPORT_FRAME_ENCODER_H

#include <outpost/base/slice.h>
#include <outpost/transport/operation_result.h>
#include <outpost/utils/expected.h>

namespace outpost
{
namespace transport
{
/**
 * Frame encoder
 *
 * The generated output byte stream can be passed directly to the UART
 * interface.
 */
class FrameEncoder
{
public:
    virtual ~FrameEncoder() = default;

    /**
     * Encode data into the destination buffer.
     *
     * \param[in] input
     *      Bytes to be sent.
     * \param[in] writeBuffer
     *      Buffer in which the encoded frame is placed.
     *
     *      Some encoders do not accept output pointing to the same buffer as input is pointing to.
     *      Encoding will fail with encodingError in this case.
     *
     * \return Expected
     * \retval true Slice
     *      Input was successfully encoded. Slice is bounded to valid data.
     * \retval false OperationResult::encodingError
     *      Encoded buffer is to big for the provided destination buffer,
     *      or input and output point to the same region,
     *      or other Encoding errors.
     */
    virtual outpost::Expected<outpost::Slice<uint8_t>, OperationResult>
    encode(outpost::Slice<const uint8_t> const& input,
           outpost::Slice<uint8_t> const& writeBuffer) = 0;
};

}  // namespace transport
}  // namespace outpost
#endif
