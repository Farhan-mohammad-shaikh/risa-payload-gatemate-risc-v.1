/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Cold, Erin Cynthia
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

/**
 * \file
 * \author Pascal Pieper
 *
 * \brief array allocator for ringbuffer
 */

#ifndef TEST_OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_IMPLEMENTATIONS_H
#define TEST_OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_IMPLEMENTATIONS_H

#include <outpost/base/testing_assert.h>
#include <outpost/container/ring_buffer_allocators/array_ring_buffer.h>

#include <unittest/container/ring_buffer_allocators/file_backed_ring_buffer.h>
#include <unittest/container/ring_buffer_allocators/persistent_file_backed_ring_buffer.h>

#include <inttypes.h>

#include <array>

namespace unittest
{

namespace container
{

struct TestInterface
{
    virtual ~TestInterface(){};
    virtual bool
    wasEverythingAccessed() const = 0;
};

template <class T>
struct MinimalAllocatorTestInterface : public T, public TestInterface
{
    template <typename... Args>
    constexpr explicit MinimalAllocatorTestInterface(Args&&... args) :
        T(std::forward<Args>(args)...)
    {
    }

    ~MinimalAllocatorTestInterface(){};

    bool
    wasEverythingAccessed() const override
    {
        // Minimal version is not checked.
        // The tests would fail otherwise, anyways.
        return true;
    }
};

using ReferencedArrayAllocatorTestInterface =
        MinimalAllocatorTestInterface<outpost::container::ReferencedArrayAllocator<uint8_t>>;

struct ReferencedArrayAllocatorMockMetadataAccessTestInterface
    : public ReferencedArrayAllocatorTestInterface
{
    constexpr explicit ReferencedArrayAllocatorMockMetadataAccessTestInterface(
            outpost::Slice<uint8_t> buffer) :
        ReferencedArrayAllocatorTestInterface(buffer),
        mReadIndex(0),
        mNumberOfElementsUsed(0),
        mWasAccessed{false}
    {
    }

    virtual ~ReferencedArrayAllocatorMockMetadataAccessTestInterface(){};

    constexpr const size_t&
    getReadIndex() const
    {
        mWasAccessed[0] = true;
        return mReadIndex;
    }

    constexpr void
    setReadIndex(const size_t& readIndex)
    {
        mWasAccessed[1] = true;
        mReadIndex = readIndex;
    }

    constexpr const size_t&
    getNumberOfElementsUsed() const
    {
        mWasAccessed[2] = true;
        return mNumberOfElementsUsed;
    }

    constexpr void
    setNumberOfElementsUsed(const size_t& numberOfElementsUsed)
    {
        mWasAccessed[3] = true;
        mNumberOfElementsUsed = numberOfElementsUsed;
    }

    virtual bool
    wasEverythingAccessed() const override
    {
        bool everything = true;
        for (const auto& accessed : mWasAccessed)
        {
            everything &= accessed;
        }
        return everything;
    }

protected:
    // implemented here to be able to save this in a different data structure
    size_t mReadIndex;
    size_t mNumberOfElementsUsed;

private:
    // not an std::array because its access is not constexpr in c++11
    mutable bool mWasAccessed[4];
};

struct ReferencedArrayAllocatorMockEverythingTestInterface
    : public ReferencedArrayAllocatorMockMetadataAccessTestInterface
{
    constexpr explicit ReferencedArrayAllocatorMockEverythingTestInterface(
            outpost::Slice<uint8_t> buffer) :
        ReferencedArrayAllocatorMockMetadataAccessTestInterface(buffer), mWasAccessed(false)
    {
    }

    virtual ~ReferencedArrayAllocatorMockEverythingTestInterface(){};

    constexpr void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        this->mReadIndex = readIndex;
        this->mNumberOfElementsUsed = numberOfElementsUsed;

        mWasAccessed = true;
    }

    virtual bool
    wasEverythingAccessed() const override
    {
        return ReferencedArrayAllocatorMockMetadataAccessTestInterface::wasEverythingAccessed()
               && mWasAccessed;
    }

private:
    bool mWasAccessed;
};

template <size_t filesize>
using FileBackedAllocatorTestInterface =
        MinimalAllocatorTestInterface<unittest::container::FileAllocator<filesize>>;

template <size_t filesize>
using PersistentFileBackedAllocatorTestInterface =
        MinimalAllocatorTestInterface<unittest::container::PersistentFileAllocator<filesize>>;

}  // namespace container

}  // namespace unittest

#endif  // TEST_OUTPOST_UTILS_RING_BUFFER_ALLOCATORS_ARRAY_IMPLEMENTATIONS_H
