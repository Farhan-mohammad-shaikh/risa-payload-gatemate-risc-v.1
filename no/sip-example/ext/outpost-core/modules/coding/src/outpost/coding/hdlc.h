/*
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_HDLC_H
#define OUTPOST_UTILS_HDLC_H

#include <outpost/base/slice.h>

#include <stdint.h>

namespace outpost
{
namespace coding
{
/**
 * HDLC (High-Level Data Link Control) encoding.
 *
 * This class implements a HDLC byte stuffing encoder and decoder.
 *
 * @author Felix Passenberg
 *
 * @see     https://en.wikipedia.org/wiki/High-Level_Data_Link_Control#Asynchronous_framing
 */
class HdlcStuffing
{
public:
    /**
     * Encode a raw byte array to HDLC byte stuffed array.
     *
     * @param [in]
     *     Input buffer of raw data to be encoded.
     * @param [out]
     *     Output buffer with the HDLC stuffed data.
     *
     * @return
     *     Number of bytes of the output data field actually used.
     */
    static size_t
    encode(outpost::Slice<const uint8_t> const& input, outpost::Slice<uint8_t>& output);

    /**
     * Decode a HDLC encoded Array to raw byte array.
     *
     * @param [in]
     *     Input buffer of the COBS encoded data to be decoded.
     * @param [out]
     *     Output buffer of raw decoded data.
     *     It is possible to do the decoding in place by supplying the same slice as input and
     *     output.
     *
     * @return
     *     position of the end Frame marker in the input array. Useful for cutting the input array
     * at this position
     */
    static size_t
    decode(outpost::Slice<const uint8_t> const& input, outpost::Slice<uint8_t>& output);

    static constexpr size_t boundary_overhead = 2;  // start and stop flags
    static constexpr uint8_t boundary_byte = 0x7E;
    static constexpr uint8_t escape_byte = 0x7D;
    static constexpr uint8_t mask = 0x20;

    // JSF++ (68): Unneeded implicitly generated member functions shall be explicitly disallowed.
    HdlcStuffing() = delete;
    ~HdlcStuffing() = delete;
    HdlcStuffing(HdlcStuffing&) = delete;
    HdlcStuffing&
    operator=(HdlcStuffing&) = delete;
};

}  // namespace coding
}  // namespace outpost

#endif
