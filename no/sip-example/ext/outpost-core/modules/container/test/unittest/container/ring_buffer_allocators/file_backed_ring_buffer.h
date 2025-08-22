/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Cold, Erin Cynthia
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
 * \brief File-based allocator for ringbuffer
 */

#ifndef UNITTEST_CONTAINER_FILE_BACKED_ALLOCATOR_H
#define UNITTEST_CONTAINER_FILE_BACKED_ALLOCATOR_H

#include <outpost/container/ring_buffer.h>

#include <inttypes.h>

#include <cstdio>  // for std::remove
#include <fstream>

namespace unittest
{

namespace container
{

/**
 * @brief Example-Implementation for an RingBuffer-Allocator that does not have the data in RAM.
 * This allows for more thorough unit-testing and serves as an implementation-example.
 */
template <size_t filesize>
struct FileAllocator
{
    explicit FileAllocator(const std::string& path = "fileRingBuffer.bin") :
        mFilename(path),
        mFile(mFilename, std::ios::in | std::ios::out | std::ios::binary | std::ios::trunc)
    {
        if (!mFile.is_open())
            throw new std::runtime_error("File '" + path + "' could not be opened!");
    }

    ~FileAllocator()
    {
        std::remove(mFilename.c_str());
    }

    constexpr void
    read(const size_t& offs, const outpost::Slice<uint8_t>& to) const
    {
        mFile.seekg(offs, std::ios::beg);
        mFile.read(reinterpret_cast<char*>(to.begin()), to.getNumberOfElements());
    }

    constexpr void
    write(const size_t& offs, const outpost::Slice<const uint8_t>& from)
    {
        mFile.seekp(offs, std::ios::beg);
        mFile.write(reinterpret_cast<const char*>(from.begin()), from.getNumberOfElements());
    }

    static constexpr size_t
    capacity()
    {
        return filesize;
    }

private:
    std::string mFilename;
    mutable std::fstream mFile;
};

template <size_t size>
using FileBackedRingbuffer = outpost::container::RingBuffer<FileAllocator<size>, uint8_t>;

}  // namespace container

}  // namespace unittest

#endif  // OUTPOST_UTILS_RINGBUFFER_ALLOCATORS_ARRAY_H
