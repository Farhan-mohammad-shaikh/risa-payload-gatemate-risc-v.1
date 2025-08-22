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

#ifndef OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_COBS_H
#define OUTPOST_TRANSPORT_BUFFERED_FRAME_DECODER_COBS_H

#include <outpost/coding/cobs.h>
#include <outpost/storage/serialize.h>
#include <outpost/transport/frame_coder/buffered_frame_decoder.h>

namespace outpost
{
namespace transport
{
/**
 * Frame decoder using COBS
 *
 */
class BufferedFrameDecoderCobs : public BufferedFrameDecoder
{
public:
    BufferedFrameDecoderCobs(outpost::Slice<uint8_t> receiveBuffer);

    BufferedFrameDecoderCobs(const BufferedFrameDecoderCobs&) = delete;

    BufferedFrameDecoderCobs&
    operator=(const BufferedFrameDecoderCobs&) = delete;

    virtual ~BufferedFrameDecoderCobs();

    /**
     * Decode a COBS encoded frame into the output buffer.
     *
     * see the parent class \c BufferedFrameDecoder.
     *
     * Applies Decoding specific validity checks.
     */
    Expected<Slice<uint8_t>, OperationResult>
    bufferedDecode(const uint8_t input, outpost::Slice<uint8_t> const& writeBuffer) override;

    void
    reset() override;

    /**
     * \brief Alias template for the worst-case sized buffer given the maximum size of the unencoded
     * frame contents
     *
     * \tparam N maximum size of frame contents
     */
    template <size_t N>
    using WorseCaseSizedBuffer =
            std::array<uint8_t, outpost::coding::CobsFrame::getMaximumSizeOfEncodedFrame(N)>;

private:
    static constexpr uint8_t flagByte = outpost::coding::cobsFrameDelimiter;

    outpost::Slice<uint8_t> mReceiveBuffer;
    outpost::Serialize mReceivedData;
};

}  // namespace transport
}  // namespace outpost
#endif
