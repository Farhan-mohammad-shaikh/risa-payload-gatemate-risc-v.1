/*
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_STORAGE_OUTPUT_STREAM_H_
#define OUTPOST_STORAGE_OUTPUT_STREAM_H_

#include <outpost/base/slice.h>

#include <stdint.h>

namespace outpost::storage
{
class OutputStream
{
public:
    OutputStream() = default;
    virtual ~OutputStream() = default;

    /**
     * Pushes data into the stream
     *
     * @param data data to push into the stream
     * @return number of bytes successfully pushed into the stream,
     *         operation successful if return value == data.getNumberOfElements()
     */
    virtual size_t
    append(const outpost::Slice<const uint8_t>& data) = 0;
};
}  // namespace outpost::storage

#endif
