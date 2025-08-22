/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Kirstein, Hannah
 * Copyright (c) 2024, Passenberg, Felix Constantin
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_COBS_H
#define OUTPOST_UTILS_COBS_H

#include <outpost/base/slice.h>

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace coding
{
/**
 * COBS (Consistent Overhead Byte Stuffing) encoding.
 *
 * This class implements a COBS encoding generator. The generator does not use
 * any additional buffer memory and outputs one byte at a time. It needs access
 * to the complete input array to calculate the COBS block sizes.
 *
 * Use this function only if you need the encoded data on a byte by byte base,
 * e.g. when loading a FIFO. The Cobs::encode() function is faster when
 * generating the data into an array in the memory.
 *
 * \author  Fabian Greif
 */
template <uint8_t blockLength>
class CobsEncodingGeneratorBase
{
public:
    /// Maximum length of a COBS block
    static const uint8_t maximumBlockLength = 254;
    static_assert(blockLength <= maximumBlockLength);

    /**
     * Construct a COBS generator object.
     *
     * \param data
     *     Input data field.
     */
    constexpr explicit CobsEncodingGeneratorBase(Slice<const uint8_t> input);

    ~CobsEncodingGeneratorBase();

    constexpr explicit CobsEncodingGeneratorBase(const CobsEncodingGeneratorBase& other);

    constexpr CobsEncodingGeneratorBase&
    operator=(const CobsEncodingGeneratorBase& other);

    constexpr bool
    isFinished() const
    {
        return (mCurrentPosition >= mData.getNumberOfElements());
    }

    constexpr uint8_t
    getNextByte();

private:
    constexpr uint8_t
    findNextBlock() const;

    Slice<const uint8_t> mData;
    size_t mCurrentPosition;
    uint8_t mNextBlock;
    bool mZeroElementSkip;
};

/**
 * COBS (Consistent Overhead Byte Stuffing) encoding and decoding.
 *
 * This class implements a modified version of the original COBS algorithm. The
 * modifications allow to specify the available output buffer space and to
 * return the number of bytes actually needed.
 *
 * \author  Fabian Greif
 *
 * \see     http://conferences.sigcomm.org/sigcomm/1997/papers/p062.pdf
 * \see     http://en.wikipedia.org/wiki/Consistent_Overhead_Byte_Stuffing
 */
template <uint8_t blockLength>
class CobsBase
{
public:
    /// Maximum length of a COBS block
    static constexpr uint8_t maximumBlockLength = 254;
    static_assert(blockLength <= maximumBlockLength);

    /**
     * Encode a byte array.
     *
     * \param [in] input
     *     Input buffer of data to be encoded.
     * \param [in out] output
     *     Output buffer. The output buffer needs to be one byte per block
     *     length longer than the input buffer. The encoding is aborted if
     *     the length of the encoded output reaches this number. The Slice
     *     is fitted to output lengths.
     *
     * \return
     *     Number of bytes of the output data field actually used. This value
     *     will never be bigger than \p maximumOutputLength.
     */
    static constexpr size_t
    encode(Slice<const uint8_t> input, Slice<uint8_t>& output);

    /**
     * The maximum number of bytes a data stream of `inputLength` can grow
     * to while encoding.
     *
     * \param [in] inputLength
     *     The length of the input data that is going to be encoded.
     *
     * \return
     *     Maximum output length possible for given inputLength.
     */
    static constexpr size_t
    getMaximumSizeOfEncodedData(size_t inputLength);

    /**
     * Decode a COBS encoded array.
     *
     * \param [in] input
     *     Input Slice of the COBS encoded data to be decoded.
     * \param [in out] output
     *     Output Slice. The buffer should have at least the same size
     *     as the input Slice. It will be cut to the valid bytes. It is
     *     possible to do the encoding in place  by supplying the same
     *     Slice as input and output.
     *
     * \return
     *     Number of bytes of the output data field actually used. Will never
     *     be bigger than \p inputLength.
     */
    static constexpr size_t
    decode(Slice<const uint8_t> input, Slice<uint8_t>& output);

    [[deprecated]] static constexpr size_t
    decode(Slice<const uint8_t> input, uint8_t* output);
};

/**
 * This class creates complete COBS frames with end markers.
 **/
class CobsFrame
{
public:
    /**
     * The maximum number of bytes a frame of `inputLength` can grow
     * to while encoding and framing.
     *
     * \param [in] inputLength
     *     The length of the input data that is going to be encoded.
     *
     * \return
     *     Maximum output length possible for given inputLength.
     */
    static constexpr size_t
    getMaximumSizeOfEncodedFrame(size_t inputLength);

    /**
     * Encode data to COBS frame and add frame delimiter.
     *
     * \param [in] input
     *      Input Slice, the data to be encoded.
     * \param [in out] output
     *      Output Slice, the memory area where the data is stored.
     *      The output is truncated to the full frame (ready to transmit).
     * \return
     *      Number of bytes of the output data field.
     */
    static constexpr size_t
    encode(const Slice<const uint8_t>& input, Slice<uint8_t>& output);

    /**
     * Decodes a COBS frame into output buffer.
     *
     * Requires the trailing boundary byte.
     *
     * \param [in out] input
     *      Input Slice, the raw byte stream to look for COBS Frames.
     *      It is truncated to the remaining data after the first COBS frame
     *      (or garbage) has been extracted.
     * \param [in out] output
     *      Output Slice, the decoded Frame data. Ready to be processed.
     *      The size is adjusted to the payload data.
     * \return
     *      Number of Bytes stripped from input. i.e. the length (inclusive) to the first 0x00 byte.
     */
    static constexpr size_t
    decode(Slice<const uint8_t>& input, Slice<uint8_t>& output);
};

typedef CobsEncodingGeneratorBase<254> CobsEncodingGenerator;
typedef CobsBase<254> Cobs;

// packet delimiter to indicate boundary between packets.
// COBS algorithm replaces this value in actual packet to make meaning of this value nonambiguous
constexpr uint8_t cobsFrameDelimiter = 0;

}  // namespace coding
}  // namespace outpost

#include "cobs_impl.h"

#endif
