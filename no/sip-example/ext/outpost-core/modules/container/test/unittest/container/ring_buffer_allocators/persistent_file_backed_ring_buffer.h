/*
 * Copyright (c) 2024, Pieper, Pascal
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
 * \author Pascal Pieper
 *
 * \brief File-based allocator for ring buffer and the convenience typedef
 */

#ifndef UNITTEST_CONTAINER_PERSISTENT_FILE_BACKED_RING_BUFFER_H
#define UNITTEST_CONTAINER_PERSISTENT_FILE_BACKED_RING_BUFFER_H

#include <outpost/container/ring_buffer.h>
#include <outpost/storage/serialize.h>

#include <inttypes.h>

#include <cstdio>  // for std::remove
#include <fstream>

// #include <iostream> // Debug output

namespace unittest
{

namespace container
{

/**
 * \brief Example-Implementation for an RingBuffer-Allocator with *persistent* storage
 * of metadata as well.
 */
template <size_t filesize>
struct PersistentFileAllocator
{
    static constexpr auto openflags = std::ios::in | std::ios::out | std::ios::binary;

    explicit PersistentFileAllocator(const std::string& path) :
        mFilename(path), mFile(mFilename, openflags)
    {
        if (!mFile.is_open())
        {
            // This also tries to create the file
            resetFile();
        }

        mFile.seekg(0, std::ios::end);
        const auto currentFilesize = mFile.tellg();

        if (currentFilesize != filesize + payloadOffset)
        {
            // std::cout << "'" << path << "' is of invalid size " << currentFilesize << ",
            // resetting buffer..." << std::endl;
            resetFile();
        }

        readMetadataFromFile();
    }

    ~PersistentFileAllocator()
    {
        syncMetadataToFile();
    }

    // Mandatory functions
    constexpr void
    read(const size_t& offs, const outpost::Slice<uint8_t>& to) const
    {
        mFile.seekg(offs + payloadOffset, std::ios::beg);
        mFile.read(reinterpret_cast<char*>(to.begin()), to.getNumberOfElements());
    }

    constexpr void
    write(const size_t& offs, const outpost::Slice<const uint8_t>& from)
    {
        mFile.seekp(offs + payloadOffset, std::ios::beg);
        mFile.write(reinterpret_cast<const char*>(from.begin()), from.getNumberOfElements());
    }

    static constexpr size_t
    capacity()
    {
        return filesize;
    }

    // Metadata access functions

    const size_t&
    getReadIndex() const
    {
        return mReadIndex;
    }

    void
    setReadIndex(const size_t& readIndex)
    {
        mReadIndex = readIndex;
        syncMetadataToFile();
    }

    const size_t&
    getNumberOfElementsUsed() const
    {
        return mNumberOfElementsUsed;
    }

    void
    setNumberOfElementsUsed(const size_t& numberOfElementsUsed)
    {
        mNumberOfElementsUsed = numberOfElementsUsed;
        syncMetadataToFile();
    }

    void
    setReadIndexAndElementsUsedAtomically(const size_t& readIndex,
                                          const size_t& numberOfElementsUsed)
    {
        mReadIndex = readIndex;
        mNumberOfElementsUsed = numberOfElementsUsed;
        syncMetadataToFile();
    }

protected:
    void
    resetFile()
    {
        mFile = std::fstream(mFilename, openflags | std::ios::trunc);
        if (!mFile.is_open())
            throw new std::runtime_error("File '" + mFilename + "' could not be created!");

        setReadIndexAndElementsUsedAtomically(0, 0);
        uint8_t dummy[1] = {0};
        write(filesize - 1, outpost::asSlice(dummy));
    }

    void
    syncMetadataToFile()
    {
        // std::cout << "Syncing data to file" << std::endl;
        std::array<uint8_t, payloadOffset> writeBuffer;
        outpost::Serialize ser(outpost::asSlice(writeBuffer));
        ser.store(mReadIndex);
        ser.store(mNumberOfElementsUsed);
        mFile.seekp(0, std::ios::beg);
        mFile.write(reinterpret_cast<char*>(writeBuffer.begin()), ser.getPosition());
        mFile.sync();
    }

    void
    readMetadataFromFile()
    {
        std::array<uint8_t, payloadOffset> readBuffer;
        mFile.seekg(0, std::ios::beg);
        mFile.read(reinterpret_cast<char*>(readBuffer.begin()), payloadOffset);
        outpost::Deserialize ser(outpost::asSlice(readBuffer));
        mReadIndex = ser.read<MetadataType>();
        mNumberOfElementsUsed = ser.read<MetadataType>();
        // std::cout << "read Metadata. ReadIndex: " << mReadIndex << ", ElementsUsed: " <<
        // mNumberOfElementsUsed << std::endl;
    }

private:
    using MetadataType = uint64_t;
    static constexpr auto bytesPerMetadataEntry = outpost::Serialize::getTypeSize<MetadataType>();
    static constexpr size_t readIndexOffset = 0 * bytesPerMetadataEntry;
    static constexpr size_t elementsUsedOffset = 1 * bytesPerMetadataEntry;
    static constexpr size_t payloadOffset = 2 * bytesPerMetadataEntry;
    std::string mFilename;
    mutable std::fstream mFile;
    MetadataType mReadIndex;
    MetadataType mNumberOfElementsUsed;
};

template <size_t size>
using PersistentFileBackedRingBuffer =
        outpost::container::RingBuffer<PersistentFileAllocator<size>, uint8_t>;

}  // namespace container

}  // namespace unittest

#endif  // UNITTEST_CONTAINER_PERSISTENT_FILE_BACKED_RING_BUFFER_H
