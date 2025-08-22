/*
 * Copyright (c) 2014-2018, Fabian Greif
 * Copyright (c) 2018, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_BITORDER_H
#define OUTPOST_UTILS_BITORDER_H

#include <stddef.h>

namespace outpost
{
/**
 * Conversion from MSB0 to LSB0 bit-ordering
 *
 * \tparam  T
 *      Target value type. Can be uint8|16|32_t.
 * \tparam startIn
 *      First bit of bitfield inside a byte array.
 * \tparam endIn
 *      Last bit of bitfield inside a byte array.
 *
 * \author  Fabian Greif
 */
template <typename T, int startIn, int endIn>
class BitorderMsb0ToLsb0
{
protected:
    static_assert(startIn < endIn,
                  "Invalid bitfield definition! 'startIn' must be smaller than 'endIn'");

    static const int numberOfBitsPerByte = 8;
    static const int msb = sizeof(T) * numberOfBitsPerByte - 1;

    static const int offsetByte = startIn & (numberOfBitsPerByte - 1);

public:
    /**
     * Byte index of the first byte of the target value in the byte array.
     */
    static const size_t byteIndex = startIn / numberOfBitsPerByte;
    static const int width = (endIn - startIn) + 1;

    /**
     * First bit of the target value inside the field given by T.
     *
     * As this is in LSB0 ordering it is always 'start > end'.
     */
    static const int start = msb - offsetByte;

    /**
     * Last bit of the target value inside the field given by T.
     */
    static const int end = msb - (endIn - (byteIndex * numberOfBitsPerByte));
};

}  // namespace outpost

#endif
