/*
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "file_system_stub.h"

#include <outpost/time/time_epoch.h>
#include <outpost/utils/minmax.h>

#include <string.h>

#include <iostream>
#include <map>

using namespace outpost;
using namespace outpost::hal;
using outpost::hal::FileSystem;
using Result = outpost::hal::FileSystem::Result;
using namespace outpost::hal::filesystem;
using namespace std;

// ---------------------------------------------------------------------------
static const std::map<Result, std::string> resultMap = {
        {Result::success, "success"},
        {Result::endOfData, "EOF or no further directory entries"},
        {Result::notFound, "File/directory not found"},
        {Result::accessDenied, "no permission"},
        {Result::noSpace, "file / directory / partition full"},
        {Result::readOnly, "cannot write"},
        {Result::writeOnly, "cannot read"},
        {Result::alreadyExists, "file/directory already exists"},
        {Result::notEmpty, "removed directory is not empty"},
        {Result::notAFile, "functions requires a file"},
        {Result::notADirectory, "function requires a directory"},
        {Result::invalidInput, "function parameter misformed"},
        {Result::resourceExhausted, "some internal resource is exhausted"},
        {Result::invalidState, "Filesytem is in a state that not allows the function to be called"},
        {Result::IOError, "Error in underlying driver/hardware"},
        {Result::notImplemented, "function not supported for this filesystem"},
        {Result::fileInUse,
         "Operation cannot be done because related file is open by other operation"},
        {Result::other, "a failure not caught by any others"},
};

static std::string
to_string(const outpost::BoundedString& from)
{
    return std::string(from.begin(), from.end());
}

namespace unittest
{
namespace hal
{

std::string
printToString(const Result& res)
{
    if (resultMap.find(res) != resultMap.end())
    {
        return resultMap.at(res);
    }
    return "UNKNOWN KEY THAT DOES NOT EXIST MAN";
}

FileSystem::Result
FileSystemStub::mount(bool readOnly)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (mMounted)
    {
        return FileSystem::Result::invalidState;
    }
    else
    {
        mMounted = true;
        mReadOnly = readOnly;
        return FileSystem::Result::success;
    }
}

FileSystem::Result
FileSystemStub::unmount()
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }
    else
    {
        mMounted = false;
        return FileSystem::Result::success;
    }
}

FileSystem::Result
FileSystemStub::mkDir(const outpost::BoundedString& path, const Permission& mask)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::readOnly;
    }

    Entry* parent = getParent(path);
    if (nullptr == parent)
    {
        return FileSystem::Result::notFound;
    }

    const auto maybePos = path.findLastOf('/');
    if (!maybePos)
    {
        return FileSystem::Result::invalidInput;
    }

    if (parent->permission.isWritable())
    {
        auto filename = path.skipFirst(*maybePos + 1);
        if (getEntryInFolder(*parent, filename) != nullptr)
        {
            return FileSystem::Result::alreadyExists;
        }

        Entry toAdd;
        toAdd.isFile = false;
        toAdd.name = to_string(filename);
        toAdd.permission = mask;
        toAdd.directoryEntries.clear();
        toAdd.fileData.clear();
        toAdd.creation =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        toAdd.modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());

        parent->directoryEntries.push_back(toAdd);
        parent->modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        return FileSystem::Result::success;
    }
    else
    {
        return FileSystem::Result::accessDenied;
    }
}

FileSystem::Result
FileSystemStub::createFile(const outpost::BoundedString& path, const Permission& permission)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::readOnly;
    }

    if (path.size() == 0U || path.size() > outpost::hal::filesystem::maxPathLength)
    {
        return Result::invalidInput;
    }

    Entry* parent = getParent(path);
    if (nullptr == parent)
    {
        return FileSystem::Result::notFound;
    }

    const auto maybePos = path.findLastOf('/');
    if (!maybePos)
    {
        return FileSystem::Result::invalidInput;
    }

    if (parent->permission.isWritable())
    {
        const auto filename = path.skipFirst(*maybePos + 1);
        if (getEntryInFolder(*parent, filename) != nullptr)
        {
            return FileSystem::Result::alreadyExists;
        }
        Entry toAdd;
        toAdd.isFile = true;
        toAdd.name = to_string(filename);
        toAdd.permission = permission;
        toAdd.directoryEntries.clear();
        toAdd.fileData.clear();
        toAdd.creation =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        toAdd.modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());

        parent->directoryEntries.push_back(toAdd);
        parent->modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        return FileSystem::Result::success;
    }
    else
    {
        return FileSystem::Result::accessDenied;
    }
}

outpost::Expected<FileSystem::Directory, FileSystem::Result>
FileSystemStub::openDir(const outpost::BoundedString& path)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    Entry* elem = getEntry(path);

    if (nullptr == elem)
    {
        return unexpected(FileSystem::Result::notFound);
    }

    // fix counting
    mCountToForce++;
    return openDir(elem);
}

outpost::Expected<FileSystem::Directory, FileSystem::Result>
FileSystemStub::openDir(DirectoryEntry& entry)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    if (entry.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    Entry* elem = reinterpret_cast<Entry*>(entry.data);

    // fix counting
    mCountToForce++;
    return openDir(elem);
}

outpost::Expected<FileSystem::Directory, FileSystem::Result>
FileSystemStub::openDir(Entry* entry)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (nullptr == entry)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    if (entry->isFile)
    {
        return unexpected(FileSystem::Result::notADirectory);
    }

    DirectoryStub* value = new DirectoryStub;
    value->directory = entry;
    value->iterator = value->directory->directoryEntries.begin();

    FileSystem::Directory dir = {reinterpret_cast<uintptr_t>(value)};
    return dir;
}

outpost::Expected<FileSystem::DirectoryEntry, FileSystem::Result>
FileSystemStub::readDir(Directory dir)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (dir.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    else
    {
        DirectoryStub* stub = reinterpret_cast<DirectoryStub*>(dir.data);
        if (stub->iterator == stub->directory->directoryEntries.end())
        {
            return unexpected(FileSystem::Result::endOfData);
        }
        else
        {
            DirectoryEntry entry = {reinterpret_cast<uintptr_t>(&(*stub->iterator))};
            ++stub->iterator;
            return entry;
        }
    }
}

outpost::Expected<outpost::BoundedString, FileSystem::Result>
FileSystemStub::getName(DirectoryEntry& entry, const outpost::Slice<char>& nameBuffer)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (entry.data == 0)
    {
        return unexpected(Result::invalidInput);
    }

    Entry* obj = reinterpret_cast<Entry*>(entry.data);

    // view on valid name bytes
    const auto maybeName = nameBuffer.concatenateCopyFrom({BoundedString{obj->name}});
    if (!maybeName)
    {
        return unexpected(Result::noSpace);
    }
    return *maybeName;
}

FileSystem::Result
FileSystemStub::closeDir(Directory& dir)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (dir.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }
    else
    {
        DirectoryStub* stub = reinterpret_cast<DirectoryStub*>(dir.data);
        dir.data = 0;
        delete stub;
        return FileSystem::Result::success;
    }
}

FileSystem::Result
FileSystemStub::rewindDir(Directory& dir)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (dir.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }
    else
    {
        DirectoryStub* stub = reinterpret_cast<DirectoryStub*>(dir.data);
        stub->iterator = stub->directory->directoryEntries.begin();
        return FileSystem::Result::success;
    }
}

outpost::Expected<FileSystem::File, FileSystem::Result>
FileSystemStub::open(const outpost::BoundedString& path, const OpenMask& mask)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    if (path.size() == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    if (mReadOnly && mask.isSet(OpenMask::WRITE()))
    {
        return unexpected(FileSystem::Result::readOnly);
    }

    Entry* entry = getEntry(path);

    if (nullptr == entry)
    {
        if (mask.isSet(OpenMask::CREATE()))
        {
            FileSystem::Result ret = createFile(path, Permission::RWX());
            if (ret != FileSystem::Result::success)
            {
                return unexpected(ret);
            }

            entry = getEntry(path);

            FileStub* stub = new FileStub;
            stub->file = entry;
            stub->pos = 0;

            if (mask.isSet(OpenMask::READ()))
            {
                stub->perm = stub->perm | Permission::R();
            }

            if (mask.isSet(OpenMask::WRITE()))
            {
                stub->perm = stub->perm | Permission::W();
            }

            if (mask.isSet(OpenMask::EXECUTE()))
            {
                stub->perm = stub->perm | Permission::X();
            }

            entry->isOpen = true;
            return File{reinterpret_cast<uintptr_t>(stub)};
        }
        else
        {
            return unexpected(FileSystem::Result::notFound);
        }
    }
    else
    {
        // fix counting
        mCountToForce++;
        return open(entry, mask);
    }
}

outpost::Expected<FileSystem::File, FileSystem::Result>
FileSystemStub::open(DirectoryEntry& dir, const OpenMask& mask)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    if (mReadOnly && (mask.isSet(OpenMask::WRITE())))
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    if (dir.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    Entry* entry = reinterpret_cast<Entry*>(dir.data);

    // fix counting
    mCountToForce++;
    return open(entry, mask);
}

outpost::Expected<FileSystem::File, FileSystem::Result>
FileSystemStub::open(Entry* entry, const OpenMask& mask)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (nullptr == entry)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    if (!entry->isFile)
    {
        return unexpected(FileSystem::Result::notAFile);
    }

    if (mask.isSet(OpenMask::READ()) && !entry->permission.isReadable())
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    if (mask.isSet(OpenMask::WRITE()) && !entry->permission.isWritable())
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    if (mask.isSet(OpenMask::WRITE()) && entry->isOpen)
    {
        return unexpected(FileSystem::Result::fileInUse);
    }

    if (mask.isSet(OpenMask::EXECUTE()) && !entry->permission.isExecutable())
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    FileStub* stub = new FileStub;
    stub->file = entry;
    if (mask.isSet(OpenMask::APPEND()))
    {
        stub->pos = entry->fileData.size();
    }
    else
    {
        stub->pos = 0;
    }

    if (mask.isSet(OpenMask::READ()))
    {
        stub->perm = stub->perm | Permission::R();
    }

    if (mask.isSet(OpenMask::WRITE()))
    {
        stub->perm = stub->perm | Permission::W();
    }

    if (mask.isSet(OpenMask::EXECUTE()))
    {
        stub->perm = stub->perm | Permission::X();
    }

    entry->isOpen = true;

    return File{reinterpret_cast<uintptr_t>(stub)};
}

FileSystem::Result
FileSystemStub::truncate(const BoundedString& path, const FileSize& newLength)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (path.getNumberOfElements() == 0)
    {
        return FileSystem::Result::invalidInput;
    }
    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    Entry* entry = getEntry(path);
    if (nullptr == entry)
    {
        return FileSystem::Result::notFound;
    }

    // fix counting
    mCountToForce++;
    return truncate(entry, newLength);
}

FileSystem::Result
FileSystemStub::truncate(DirectoryEntry& file, const FileSize& newLength)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    if (file.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }

    Entry* entry = reinterpret_cast<Entry*>(file.data);

    // fix counting
    mCountToForce++;
    return truncate(entry, newLength);
}

FileSystem::Result
FileSystemStub::truncate(Entry* entry, const FileSize& newLength)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (nullptr == entry)
    {
        return FileSystem::Result::invalidInput;
    }
    if (!entry->isFile)
    {
        return FileSystem::Result::notAFile;
    }

    if (entry->isOpen)
    {
        return FileSystem::Result::fileInUse;
    }

    if (!entry->permission.isWritable())
    {
        return FileSystem::Result::accessDenied;
    }

    if (entry->fileData.size() < newLength)
    {
        return FileSystem::Result::invalidInput;
    }

    entry->fileData.resize(newLength);
    entry->modify =
            outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                              outpost::time::GpsEpoch>::convert(mClock.now());
    return FileSystem::Result::success;
}

FileSystem::Result
FileSystemStub::close(File& file)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (file.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }
    else
    {
        FileStub* stub = reinterpret_cast<FileStub*>(file.data);
        stub->file->isOpen = false;
        file.data = 0;
        delete stub;
        return FileSystem::Result::success;
    }
}

FileSystem::MaybeSlice
FileSystemStub::read(File& file, const outpost::Slice<uint8_t>& data)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return outpost::unexpected(FileSystem::Result::invalidState);
    }

    if (file.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    FileStub* stub = reinterpret_cast<FileStub*>(file.data);
    if (!stub->perm.isReadable())
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    // read zero bytes is always successful even if no data is available
    if (data.getNumberOfElements() == 0U)
    {
        return outpost::emptySlice();
    }

    if (stub->file->fileData.size() <= stub->pos)
    {
        return unexpected(FileSystem::Result::endOfData);
    }

    uint64_t readSize = outpost::utils::min<uint64_t>(data.getNumberOfElements(),
                                                      stub->file->fileData.size() - stub->pos);

    const auto maybeReadDataSlice =
            data.concatenateCopyFrom({asSliceUnsafe(&stub->file->fileData[stub->pos], readSize)});
    OUTPOST_ASSERT(maybeReadDataSlice.has_value(), "Internal, unexpected error");
    stub->pos += readSize;
    return maybeReadDataSlice.value();
}

FileSystem::MaybeNumberOfBytes
FileSystemStub::write(File& file, const outpost::Slice<const uint8_t>& data)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return outpost::unexpected(FileSystem::Result::invalidState);
    }

    // `open` should already fail, but to be sure:
    if (mReadOnly)
    {
        return outpost::unexpected(Result::readOnly);
    }

    if (file.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }

    FileStub* stub = reinterpret_cast<FileStub*>(file.data);

    if (!stub->perm.isWritable())
    {
        return unexpected(FileSystem::Result::accessDenied);
    }

    if (stub->file->fileData.size() < data.getNumberOfElements() + stub->pos)
    {
        stub->file->fileData.resize(data.getNumberOfElements() + stub->pos);
    }

    if (data.getNumberOfElements() > 0U)
    {
        asSlice(stub->file->fileData).skipFirst(stub->pos).copyFrom(data);
        stub->pos += data.getNumberOfElements();
    }

    stub->file->modify =
            outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                              outpost::time::GpsEpoch>::convert(mClock.now());
    // always succeeds completely in our stub
    return data.getNumberOfElements();
}

FileSystem::Result
FileSystemStub::seek(File& file, const FileSizeDiff& diff, const SeekMode& mode)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (file.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }

    FileStub* stub = reinterpret_cast<FileStub*>(file.data);

    int64_t newPos = 0;

    switch (mode)
    {
        case SeekMode::set: newPos = diff; break;
        case SeekMode::current: newPos = diff + stub->pos; break;
        case SeekMode::end: newPos = diff + stub->file->fileData.size(); break;
    }

    // cast is safe as if negative the first part already hits
    if (newPos < 0 || static_cast<uint64_t>(newPos) > stub->file->fileData.size())
    {
        return FileSystem::Result::invalidInput;
    }
    else
    {
        stub->pos = newPos;
        return FileSystem::Result::success;
    }
}

outpost::Expected<FileSystem::Info, FileSystem::Result>
FileSystemStub::getInfo(const outpost::BoundedString& path)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (path.size() == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    Entry* entry = getEntry(path);
    if (nullptr == entry)
    {
        return unexpected(FileSystem::Result::notFound);
    }
    return Info{reinterpret_cast<uintptr_t>(entry)};
}

outpost::Expected<FileSystem::Info, FileSystem::Result>
FileSystemStub::getInfo(DirectoryEntry& entry)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (!mMounted)
    {
        return unexpected(FileSystem::Result::invalidState);
    }

    if (entry.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    return Info{entry.data};
}

FileSystem::Result
FileSystemStub::releaseInfo(Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    info.data = 0;
    return FileSystem::Result::success;
}

outpost::Expected<bool, FileSystem::Result>
FileSystemStub::isFile(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    return entry->isFile;
}

outpost::Expected<bool, FileSystem::Result>
FileSystemStub::isDirectory(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    return !entry->isFile;
}

outpost::Expected<FileSize, FileSystem::Result>
FileSystemStub::getSize(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    if (!entry->isFile)
    {
        return unexpected(FileSystem::Result::notAFile);
    }
    return entry->fileData.size();
}

outpost::Expected<Permission, FileSystem::Result>
FileSystemStub::getPermissions(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    return entry->permission;
}

outpost::Expected<outpost::time::GpsTime, FileSystem::Result>
FileSystemStub::getCreationTime(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    return entry->creation;
}

outpost::Expected<outpost::time::GpsTime, FileSystem::Result>
FileSystemStub::getModifyTime(const Info& info)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return unexpected(tmp);
        }
    }

    if (info.data == 0)
    {
        return unexpected(FileSystem::Result::invalidInput);
    }
    auto entry = reinterpret_cast<const Entry*>(info.data);
    return entry->modify;
}

FileSystem::Result
FileSystemStub::chmod(const BoundedString& path, const Permission& perm)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    Entry* entry = getEntry(path);
    if (nullptr == entry)
    {
        return FileSystem::Result::notFound;
    }

    entry->permission = perm;

    return FileSystem::Result::success;
}

FileSystem::Result
FileSystemStub::chmod(DirectoryEntry& obj, const Permission& perm)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    Entry* entry = reinterpret_cast<Entry*>(obj.data);
    if (nullptr == entry)
    {
        return FileSystem::Result::invalidInput;
    }

    entry->permission = perm;

    return FileSystem::Result::success;
}

FileSystem::Result
FileSystemStub::remove(const BoundedString& path)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    Entry* entry = getEntry(path);

    if (nullptr == entry)
    {
        return FileSystem::Result::notFound;
    }

    if (!(entry->permission.isWritable()))
    {
        return FileSystem::Result::accessDenied;
    }

    if (checkOpenedRecursive(*entry))
    {
        return FileSystem::Result::fileInUse;
    }

    Entry* parent = getParent(path);
    if (nullptr == parent)
    {
        return FileSystem::Result::invalidInput;  // tried removing root
    }

    if (!entry->isFile && entry->directoryEntries.size() > 0)
    {
        return FileSystem::Result::notEmpty;
    }

    if (!(parent->permission.isWritable()))
    {
        return FileSystem::Result::accessDenied;
    }

    auto it = std::find_if(parent->directoryEntries.begin(),
                           parent->directoryEntries.end(),
                           [entry](Entry& p) { return &p == entry; });
    if (it != parent->directoryEntries.end())
    {
        parent->directoryEntries.erase(it);
        parent->modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        return FileSystem::Result::success;
    }

    // can only happen if we have concurrent access
    return FileSystem::Result::other;
}

FileSystem::Result
FileSystemStub::remove(Directory& p, DirectoryEntry& e)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    if (p.data == 0 || e.data == 0)
    {
        return FileSystem::Result::invalidInput;
    }

    Entry* entry = reinterpret_cast<Entry*>(e.data);

    if (checkOpenedRecursive(*entry))
    {
        return FileSystem::Result::fileInUse;
    }

    if (!(entry->permission.isWritable()))
    {
        return FileSystem::Result::accessDenied;
    }

    Entry* parent = reinterpret_cast<DirectoryStub*>(p.data)->directory;

    if (!entry->isFile && entry->directoryEntries.size() > 0)
    {
        return FileSystem::Result::notEmpty;
    }

    if (!(parent->permission.isWritable()))
    {
        return FileSystem::Result::accessDenied;
    }

    auto it = std::find_if(parent->directoryEntries.begin(),
                           parent->directoryEntries.end(),
                           [entry](Entry& other) { return &other == entry; });
    if (it != parent->directoryEntries.end())
    {
        parent->directoryEntries.erase(it);
        parent->modify =
                outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                  outpost::time::GpsEpoch>::convert(mClock.now());
        return FileSystem::Result::success;
    }

    // p was not parent of e
    return FileSystem::Result::invalidInput;
}

FileSystem::Result
FileSystemStub::rename(const outpost::BoundedString& sourcePath,
                       const outpost::BoundedString& targetPath)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    FileSystem::Result r = copy(sourcePath, targetPath);
    if (r != FileSystem::Result::success)
    {
        return r;
    }

    // fix counting
    mCountToForce++;
    return remove(sourcePath);
}

FileSystem::Result
FileSystemStub::copy(const outpost::BoundedString& sourcePath,
                     const outpost::BoundedString& targetPath)
{
    if (mForcedResult != Result::success)
    {
        if (mCountToForce > 0)
        {
            mCountToForce--;
        }
        else
        {
            Result tmp = mForcedResult;
            mForcedResult = Result::success;
            return tmp;
        }
    }

    if (!mMounted)
    {
        return FileSystem::Result::invalidState;
    }

    if (mReadOnly)
    {
        return FileSystem::Result::accessDenied;
    }

    const Entry* source = getEntry(sourcePath);
    if (nullptr == source)
    {
        return FileSystem::Result::notFound;
    }

    const Entry* target = getEntry(targetPath);
    if (nullptr != target)
    {
        return FileSystem::Result::alreadyExists;
    }

    Entry* targetDir = getParent(targetPath);
    if (nullptr == targetDir)
    {
        return FileSystem::Result::notFound;
    }

    if (targetDir->isFile)
    {
        return FileSystem::Result::notADirectory;
    }

    if (!(targetDir->permission.isWritable()))
    {
        return FileSystem::Result::accessDenied;
    }

    Entry newEntry = *source;
    std::string path = to_string(targetPath);
    size_t pos = path.find_last_of('/');
    std::string name = path.substr(pos + 1);
    updateTime(newEntry,
               outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                 outpost::time::GpsEpoch>::convert(mClock.now()));
    newEntry.name = name;
    targetDir->directoryEntries.push_back(newEntry);
    targetDir->modify =
            outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                              outpost::time::GpsEpoch>::convert(mClock.now());

    return FileSystem::Result::success;
}

void
FileSystemStub::updateTime(Entry& entry, outpost::time::GpsTime time)
{
    entry.modify = time;
    entry.creation = time;

    if (!entry.isFile)
    {
        for (auto it = entry.directoryEntries.begin(); it != entry.directoryEntries.end(); ++it)
        {
            updateTime(*it, time);
        }
    }
}

bool
FileSystemStub::existsFile(const outpost::BoundedString& path)
{
    const Entry* entry = getEntry(path);
    return entry != nullptr && entry->isFile;
}

bool
FileSystemStub::existsDirectory(const outpost::BoundedString& path)

{
    const Entry* entry = getEntry(path);
    return entry != nullptr && !entry->isFile;
}

outpost::Expected<Permission, FileSystem::Result>
FileSystemStub::getPermission(const outpost::BoundedString& path)
{
    const Entry* entry = getEntry(path);
    if (nullptr == entry)
    {
        return unexpected(Result::notFound);
    }
    return entry->permission;
}

bool
FileSystemStub::isFileContentEquals(const outpost::BoundedString& path,
                                    const outpost::Slice<const uint8_t>& data)
{
    const Entry* entry = getEntry(path);
    if (entry == nullptr || !entry->isFile)
    {
        return false;
    }

    if (entry->fileData.size() != data.getNumberOfElements())
    {
        return false;
    }

    if (!data.getNumberOfElements())
    {
        // two empty-sized buffers are the same
        return true;
    }

    return memcmp(&entry->fileData[0], data.getDataPointer(), data.getNumberOfElements()) == 0;
}

void
FileSystemStub::reset()
{
    mRoot = Entry();
    mRoot.name = "rootnode";
}

FileSystemStub::Entry*
FileSystemStub::getEntryInFolder(FileSystemStub::Entry& dir, const outpost::BoundedString& name)
{
    if (dir.isFile)
    {
        return nullptr;
    }
    const auto stringName = to_string(name);
    auto it = std::find_if(dir.directoryEntries.begin(),
                           dir.directoryEntries.end(),
                           [stringName](const Entry& p) { return p.name == stringName; });
    if (it != dir.directoryEntries.end())
    {
        return &(*it);
    }
    return nullptr;
}

FileSystemStub::Entry*
FileSystemStub::getEntry(const outpost::BoundedString& path)
{
    if (path[0] != '/')
    {
        // not a valid path
        return nullptr;
    }

    if (path == "/")
    {
        return &mRoot;
    }

    FileSystemStub::Entry* parent = getParent(path);
    if (nullptr == parent)
    {
        return nullptr;
    }
    else
    {
        const auto maybePos = path.findLastOf('/');
        if (!maybePos)
        {
            return nullptr;
        }
        return getEntryInFolder(*parent, path.skipFirst(*maybePos + 1));
    }
}

FileSystemStub::Entry*
FileSystemStub::getParent(const outpost::BoundedString& path)
{
    if (path[0] != '/')
    {
        // not a valid path
        return nullptr;
    }

    BoundedString pathView = path.skipFirst(1);  // remove leading '/'

    FileSystemStub::Entry* start = &mRoot;
    while (nullptr != start)
    {
        auto maybeIndex = pathView.findFirstOf('/');
        if (!maybeIndex)
        {
            return start;
        }

        FileSystemStub::Entry* next = getEntryInFolder(*start, pathView.first(*maybeIndex));
        if (next == nullptr || next->isFile)
        {
            return nullptr;
        }

        start = next;
        pathView = pathView.skipFirst(*maybeIndex + 1);  // also skip slash
    }
    return nullptr;
}

bool
FileSystemStub::checkOpenedRecursive(Entry& entry)
{
    if (entry.isFile)
    {
        return entry.isOpen;
    }
    else
    {
        auto it = std::find_if(entry.directoryEntries.begin(),
                               entry.directoryEntries.end(),
                               [](Entry& p) { return checkOpenedRecursive(p); });
        if (it != entry.directoryEntries.end())
        {
            return true;
        }
        return false;
    }
}

void
FileSystemStub::printFileSystemTree(const Entry& entry, const size_t level)
{
    const std::string indent(level, ' ');
    std::cout << indent << "- " << entry.name;
    if (!entry.isFile)
        std::cout << "/";
    std::cout << " ";
    if (entry.isOpen)
        std::cout << "(open)";
    std::cout << std::endl;

    for (const auto& sub : entry.directoryEntries)
    {
        printFileSystemTree(sub, level + 1);
    }
}

}  // namespace hal
}  // namespace unittest
