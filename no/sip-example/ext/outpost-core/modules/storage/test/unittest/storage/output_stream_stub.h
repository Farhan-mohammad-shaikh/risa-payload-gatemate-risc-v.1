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

#ifndef UNITTEST_STORAGE_OUTPUT_STREAM_STUB_H_
#define UNITTEST_STORAGE_OUTPUT_STREAM_STUB_H_

#include <outpost/storage/output_stream.h>

#include <vector>

namespace unittest
{
namespace storage
{
class OutputStreamStub : public outpost::storage::OutputStream
{
public:
    OutputStreamStub() = default;

    size_t
    append(const outpost::Slice<const uint8_t>& data) override;

    /**
     * @return all appended data
     */
    std::vector<uint8_t>&
    getData()
    {
        return mData;
    }

    /**
     * Set a limit of how many elements at most to append
     * @param limit the new limit
     */
    void
    setLimit(size_t limit)
    {
        mLimit = limit;
    }

    void
    clear()
    {
        mData.clear();
    }

private:
    std::vector<uint8_t> mData;
    size_t mLimit = SIZE_MAX;
};
}  // namespace storage
}  // namespace unittest

#endif
