/*
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022-2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_HAL_FILE_SYSTEM_STUB_H_
#define UNITTEST_HAL_FILE_SYSTEM_STUB_H_

#include <outpost/hal/file_system.h>
#include <outpost/time/clock.h>

#include <list>
#include <string>
#include <vector>

namespace unittest
{
namespace hal
{

// Unfortunately outpost_hal_stubs can not depend on GTest since it is used
// also for system tests. If you want more advanced error messages from GTest
// add the following code to your test:
//
// #include <gtest/gtest.h>
//
// template <>
// std::string
// testing::PrintToString(const outpost::hal::FileSystem::Result& res)
// {
//     return unittest::hal::printToString(res);
// }

std::string
printToString(const outpost::hal::FileSystem::Result& res);

class FileSystemStub : public outpost::hal::FileSystem
{
    /**
     * To let the std::vector handle all the heap stuff we have an entry that can be either a file
     * or a folder
     */
    struct Entry
    {
        std::string name = "";
        outpost::hal::filesystem::Permission permission =
                outpost::hal::filesystem::Permission::RWX();
        bool isFile = false;
        std::list<Entry> directoryEntries;
        std::vector<uint8_t> fileData;
        outpost::time::GpsTime creation = outpost::time::GpsTime::startOfEpoch();
        outpost::time::GpsTime modify = outpost::time::GpsTime::startOfEpoch();
        bool isOpen = false;
    };

public:
    explicit FileSystemStub(outpost::time::Clock& clock) :
        mClock(clock), mForcedResult(Result::success), mCountToForce(0)
    {
        reset();
    }

    bool
    isMounted() const override
    {
        return mMounted;
    }

    Result
    mount(bool readOnly) override;

    Result
    unmount() override;

    Result
    mkDir(const outpost::BoundedString& path,
          const outpost::hal::filesystem::Permission& mask) override;

    MaybeDirectory
    openDir(const outpost::BoundedString& path) override;

    MaybeDirectory
    openDir(DirectoryEntry& entry) override;

    MaybeDirectoryEntry
    readDir(Directory dir) override;

    MaybeBoundedString
    getName(DirectoryEntry& entry, const outpost::Slice<char>& nameBuffer) override;

    Result
    closeDir(Directory& dir) override;

    Result
    rewindDir(Directory& dir) override;

    MaybeFile
    open(const outpost::BoundedString& path,
         const outpost::hal::filesystem::OpenMask& mask) override;

    MaybeFile
    open(DirectoryEntry& dir, const outpost::hal::filesystem::OpenMask& mask) override;

    Result
    createFile(const outpost::BoundedString& path,
               const outpost::hal::filesystem::Permission& permission) override;

    Result
    close(File& file) override;

    MaybeSlice
    read(File& file, const outpost::Slice<uint8_t>& data) override;

    MaybeNumberOfBytes
    write(File& file, const outpost::Slice<const uint8_t>& data) override;

    Result
    seek(File& file,
         const outpost::hal::filesystem::FileSizeDiff& diff,
         const SeekMode& mode) override;

    Result
    truncate(const outpost::BoundedString& path,
             const outpost::hal::filesystem::FileSize& newLength) override;

    Result
    truncate(DirectoryEntry& file, const outpost::hal::filesystem::FileSize& newLength) override;

    MaybeInfo
    getInfo(const outpost::BoundedString& path) override;

    MaybeInfo
    getInfo(DirectoryEntry& entry) override;

    Result
    releaseInfo(Info& info) override;

    MaybeBool
    isFile(const Info& info) override;

    MaybeBool
    isDirectory(const Info& info) override;

    MaybeNumberOfBytes
    getSize(const Info& info) override;

    MaybePermission
    getPermissions(const Info& info) override;

    MaybeTime
    getCreationTime(const Info& info) override;

    MaybeTime
    getModifyTime(const Info& info) override;

    Result
    chmod(const outpost::BoundedString& path,
          const outpost::hal::filesystem::Permission& perm) override;

    Result
    chmod(DirectoryEntry& obj, const outpost::hal::filesystem::Permission& perm) override;

    Result
    remove(const outpost::BoundedString& path) override;

    Result
    remove(Directory& p, DirectoryEntry& e) override;

    Result
    rename(const outpost::BoundedString& sourcePath,
           const outpost::BoundedString& targetPath) override;

    Result
    copy(const outpost::BoundedString& sourcePath,
         const outpost::BoundedString& targetPath) override;

    Result
    flush(File&) override
    {
        return Result::success;
    }

    Result
    flush() override
    {
        return Result::success;
    }

    /**
     * Will cause an error after a given amount of calls.
     * Only one may be queued at a time, set another will override the old
     *
     * @param error the error which should be caused, Result::success will disable
     * @param delay how many calls between now and the error is allowed
     */
    void
    forceError(Result error, size_t delay)
    {
        mForcedResult = error;
        mCountToForce = delay;
    }

    // some helper function for unit test
    /**
     * checks whether file exist
     * @param path the path to file
     * @return true if path exists and is a file
     */
    bool
    existsFile(const outpost::BoundedString& path);

    /**
     * checks whether directory exist
     * @param path the path to directory
     * @return true if path exists and is a directory
     */
    bool
    existsDirectory(const outpost::BoundedString& path);

    /**
     * checks whether file exist with an defined content
     * @param path the path to file
     * @param the expected contents of the file
     * @return true if path exists, is a file and it data is as expected
     */
    bool
    isFileContentEquals(const outpost::BoundedString& path,
                        const outpost::Slice<const uint8_t>& data);

    /**
     * Provided the permission of a given entry
     * @param path the path to file/dirctory
     * @param perm place to store der permission
     * @return true if path exists
     */
    outpost::Expected<outpost::hal::filesystem::Permission, Result>
    getPermission(const outpost::BoundedString& path);

    /**
     * restarts with empty file system, not only use when all handles are freed
     */
    void
    reset();

private:
    struct DirectoryStub
    {
        Entry* directory;
        std::list<Entry>::iterator iterator;
    };

    struct FileStub
    {
        Entry* file;
        outpost::hal::filesystem::Permission perm;
        outpost::hal::filesystem::FileSize pos;
    };

    /**
     * Opens a file
     *
     * @param entry the file to open
     * @param mask flags for opening
     * @return \c File on success, or Operation result
     */
    outpost::Expected<File, Result>
    open(Entry* entry, const outpost::hal::filesystem::OpenMask& mask);

    /**
     * Opens a directory to iterate its content
     *
     * @param entry the folder to open
     * @return \c Directory on success, or Operation result
     */
    outpost::Expected<Directory, Result>
    openDir(Entry* entry);

    /**
     * Sets the size of a file to a given value
     * implementation supports size reduction only
     *
     * @param entry the file to truncate
     * @param newLength new length of the file
     * @return Operation result
     */
    Result
    truncate(Entry* entry, const outpost::hal::filesystem::FileSize& newLength);

    /**
     * Get the Entry of a given name in a folder
     * @param dir the director to search
     * @param name the name
     * @return Pointer to do entry or nullptr if not exists
     */
    static Entry*
    getEntryInFolder(Entry& dir, const outpost::BoundedString& name);

    /**
     * Get the Entry for a given path
     * @param path the path to search
     * @return Pointer to do entry or nullptr if not exists
     */
    Entry*
    getEntry(const outpost::BoundedString& path);

    /**
     * Get the parent directory for a given absolute path
     * @param path the path to search
     * @return Pointer to do entry or nullptr if not exists
     */
    Entry*
    getParent(const outpost::BoundedString& path);

    /**
     * Set all time of entry and child to a given time
     * @param entry the entry
     * @param time the time to set
     */
    static void
    updateTime(Entry& entry, outpost::time::GpsTime time);

    /**
     * Checks recursive whether a element or one of its children are opended.
     *
     * @param entry The entry to check
     * @return true if element or any of its children is opened
     */
    static bool
    checkOpenedRecursive(Entry& entry);

    /**
     * Helper with debugging
     */
    void
    printFileSystemTree(const Entry& entry, const size_t level = 0);

    bool mMounted = false;
    bool mReadOnly = false;
    Entry mRoot;  // default values of entry are fine.

    outpost::time::Clock& mClock;

    Result mForcedResult;
    size_t mCountToForce;
};

}  // namespace hal
}  // namespace unittest

#endif
