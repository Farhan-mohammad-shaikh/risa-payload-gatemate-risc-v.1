/*
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 * Copyright (c) 2024, Meß, Jan-Gerd
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_FILE_SYSTEM_H_
#define OUTPOST_HAL_FILE_SYSTEM_H_

#include <outpost/base/bounded_string.h>
#include <outpost/base/slice.h>
#include <outpost/time.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/safe_mask.h>

#include <array>

// max path length et al
#include <outpost/parameter/file_system.h>

#include <stdint.h>

namespace outpost
{
namespace hal
{
namespace filesystem
{
typedef uint64_t FileSize;
typedef int64_t FileSizeDiff;
/// convenience alias for "worst case" path length buffer
typedef std::array<char, outpost::hal::filesystem::maxPathLength> PathBuffer;

class Permission : public Mask<Permission>
{
public:
    // conversion-constructor to parent
    // cppcheck-suppress noExplicitConstructor
    constexpr Permission(const Mask<Permission>& mask) : Mask<Permission>(mask)
    {
    }

    // Helper aliases
    constexpr bool
    isWritable() const
    {
        return isSet(Permission::W());
    }

    constexpr bool
    isReadable() const
    {
        return isSet(Permission::R());
    }

    constexpr bool
    isExecutable() const
    {
        return isSet(Permission::X());
    }

    // "globals"

    /**
     * Permission to read object
     */
    static constexpr const Permission
    R()
    {
        return Permission(0b001);
    }

    /**
     * Permission to write object
     */
    static constexpr const Permission
    W()
    {
        return Permission(0b010);
    }

    /**
     * Permission to execute code from object
     */
    static constexpr const Permission
    X()
    {
        return Permission(0b100);
    }

    /**
     * Convenience: Permissive Permissions
     */
    static constexpr const Permission
    RWX()
    {
        return R() | W() | X();
    }

private:
    using Mask<Permission>::Mask;
};

class OpenMask : public Mask<OpenMask>
{
public:
    // conversion-constructor to parent
    // cppcheck-suppress noExplicitConstructor
    constexpr OpenMask(const Mask<OpenMask>& mask) : Mask<OpenMask>(mask)
    {
    }

    // aliases
    static constexpr OpenMask
    READ()
    {
        return OpenMask{0b00001};
    }

    static constexpr OpenMask
    WRITE()
    {
        return OpenMask{0b00010};
    }

    static constexpr OpenMask
    EXECUTE()
    {
        return OpenMask{0b00100};
    }

    static constexpr OpenMask
    APPEND()
    {
        return OpenMask{0b01000};
    }

    static constexpr OpenMask
    CREATE()
    {
        return OpenMask{0b10000};
    }

private:
    using Mask<OpenMask>::Mask;
};
}  // namespace filesystem

class FileSystem
{
public:
    // TODO: Move all definitions into separate namespace?
    /**
     * A general result list, that should include most used result types
     */
    enum class Result : uint8_t
    {
        success,
        endOfData,          // EOF or no further directory entries
        notFound,           // File/directory not found
        accessDenied,       // no permission
        noSpace,            // file / directory / partition full
        readOnly,           // cannot write
        writeOnly,          // cannot read
        alreadyExists,      // file/directory already exists
        notEmpty,           // removed directory is not empty
        notAFile,           // functions requires a file
        notADirectory,      // function requires a directory
        invalidInput,       // function parameter misformed
        resourceExhausted,  // some internal resource is exhausted
        invalidState,       // Filesytem is in a state that not allows the function to be called
        IOError,            // Error in underlying driver/hardware
        notImplemented,     // function not supported for this filesystem
        fileInUse,   // Operation cannot be done because related file is open by other operation
        notMounted,  // usage of fs without previously mounting it
        other        // a failure not caught by any above
    };

    // usage of data implementation specific.
    // put enough memory space in that a pointer can be used
    struct Directory
    {
        uintptr_t data = 0;
    };

    struct DirectoryEntry
    {
        uintptr_t data = 0;
    };

    struct File
    {
        uintptr_t data = 0;
    };

    struct Info
    {
        uintptr_t data = 0;
    };

    enum class SeekMode : uint_fast8_t
    {
        set,
        current,
        end
    };

    typedef outpost::Expected<outpost::Slice<const uint8_t>, Result> MaybeSlice;
    typedef outpost::Expected<filesystem::FileSize, Result> MaybeNumberOfBytes;
    typedef outpost::Expected<filesystem::Permission, Result> MaybePermission;
    typedef outpost::Expected<outpost::time::GpsTime, Result> MaybeTime;
    typedef outpost::Expected<Info, Result> MaybeInfo;
    typedef outpost::Expected<File, Result> MaybeFile;
    typedef outpost::Expected<Directory, Result> MaybeDirectory;
    typedef outpost::Expected<DirectoryEntry, Result> MaybeDirectoryEntry;
    typedef outpost::Expected<outpost::BoundedString, Result> MaybeBoundedString;
    typedef outpost::Expected<bool, Result> MaybeBool;
    typedef outpost::Slice<char> StringBufferSlice;  // TODO: Move this into BoundedString?
    template <size_t n>
    using StringBuffer = std::array<char, n>;

    FileSystem() = default;
    virtual ~FileSystem() = default;

    FileSystem&
    operator=(const FileSystem&) = delete;
    FileSystem(const FileSystem&) = delete;

    /**
     * @return whether the filesystem is mounted
     */
    virtual bool
    isMounted() const = 0;

    /**
     * Mounts the filesystem
     *
     * @param readOnly If true the filesystem cannot be modified
     * @return Operation result
     */
    virtual Result
    mount(bool readOnly) = 0;

    /**
     * Unmounts the filesystem
     *
     * @return Operation result
     */
    virtual Result
    unmount() = 0;

    /**
     * Creates a directory at a given path
     *
     * @param path the absolute path of the directory
     * @param mask the permission with which the folder should be created
     * @return Operation result
     */
    virtual Result
    mkDir(const BoundedString& path,
          const filesystem::Permission& mask = filesystem::Permission::RWX()) = 0;

    /**
     * Opens a directory to iterate its content
     *
     * @param path path of the directory
     * @return \c Directory on success, or Operation result
     */
    virtual MaybeDirectory
    openDir(const BoundedString& path) = 0;

    /**
     * Opens a directory to iterate its content
     *
     * @param entry entry to open
     * @return \c Directory on success, or Operation result
     */
    virtual MaybeDirectory
    openDir(DirectoryEntry& entry) = 0;

    /**
     * Gives the next element in a directory
     *
     * @param dir directory to iterate
     * @return \c DirectoryEntry on success, or Operation result
     *         \c Result::endOfData : directory iteration already done
     */
    virtual MaybeDirectoryEntry
    readDir(Directory dir) = 0;

    /**
     * Get the name of a directory entry.
     *
     * @param entry the entry for which the name should be requested
     * @param buffer place to store the position of the name.
     *        Useful type for that is \c filesystem::PathBuffer .
     * @return \c BoundedString with up to \c length(name) elements,
     *         or else Operation result
     * @warning if @param name is smaller than the actual name,
     *          it might return a truncated \c BoundedString
     *          or return an error Operation Result
     */
    virtual MaybeBoundedString
    getName(DirectoryEntry& entry, const StringBufferSlice& nameBuffer) = 0;

    /**
     * Closes a directory, also closes the corresponding DirectoryEntry objects
     * @param dir the directory to close
     * @return Operation result
     */
    virtual Result
    closeDir(Directory& dir) = 0;

    /**
     * Resets the iteration pointer for a directory
     * @param dir Directory to reset its iteration
     * @return Operation result
     */
    virtual Result
    rewindDir(Directory& dir) = 0;

    /**
     * Opens a file
     *
     * @param path absolute path to the file
     * @param mask flags for opening
     * @return \c File on success, or Operation result
     */
    virtual MaybeFile
    open(const BoundedString& path, const filesystem::OpenMask& mask) = 0;

    /**
     * Opens a file
     *
     * @param dir File entry pointing to the file to open
     * @param mask flags for opening
     * @return \c File on success, or Operation result
     */
    virtual MaybeFile
    open(DirectoryEntry& dir, const filesystem::OpenMask& mask) = 0;

    /**
     * \brief Creates a new file
     *
     * \param path
     *      absolute path to the file
     * \param permission
     *      the permission of the new file
     *
     * \retval success, if the file could be created
     * \retval alreadyExists, if the file/folder already exists
     * \retval notFound, if the parent folder of \p path does not exist
     * \retval invalidArgument, if \p path is empty or exceeds max path length
     * \retval invalidState, if the file system was not mounted
     * \retval readOnly, if the file system is read-only
     * \retval accessDenied, if the parent folder is read-only
     */
    virtual Result
    createFile(const BoundedString& path, const filesystem::Permission& permission) = 0;

    /**
     * Closes a file
     *
     * @param file the file to close
     * @return Operation result
     */
    virtual Result
    close(File& file) = 0;

    /**
     * Gets the information object for a file/directory
     *
     * @param path absolute path of the element to open
     * @return \c Info on success, or Operation result
     */
    virtual MaybeInfo
    getInfo(const BoundedString& path) = 0;

    /**
     * Gets the information object for a file/directory
     *
     * @param entry Entry pointing to the element to get the information
     * @return \c Info on success, or Operation result
     */
    virtual MaybeInfo
    getInfo(DirectoryEntry& entry) = 0;

    /**
     * Frees the resources of a info object
     *
     * @param info the information object to free
     * @return Operation result
     */
    virtual Result
    releaseInfo(Info& info) = 0;

    /**
     * Checks whether a info points to a file
     *
     * @param info
     *        the info to check
     * @return
     *      Answer on success, or Operation result
     * @warning
     *      Expected is also bool overloaded! Use \c has_value() instead.
     */
    virtual MaybeBool
    isFile(const Info& info) = 0;

    /**
     * Checks whether a info points to a directory
     *
     * @param info
     *        the info to check
     * @return
     *      Answer on success, or Operation result
     * @warning
     *      Expected is also bool overloaded! Use \c has_value() instead.
     */
    virtual MaybeBool
    isDirectory(const Info& info) = 0;

    /**
     * Gets the size of a file
     *
     * @param info the info to check
     * @return \c Filesize on Success, or Operation result
     */
    virtual MaybeNumberOfBytes
    getSize(const Info& info) = 0;

    /**
     * Gets the permission for a file or directory
     *
     * @param info the info to check
     * @return \c Permission on success, or OperationResult
     */
    virtual MaybePermission
    getPermissions(const Info& info) = 0;

    /**
     * Gets the creation time of an element
     *
     * @param info the info to check
     * @return \c GpsTime on success, or Operation result
     */
    virtual MaybeTime
    getCreationTime(const Info& info) = 0;

    /**
     * Gets the modify time of an element
     *
     * @param info the info to check
     * @return \c GpsTime on success, or Operation result
     */
    virtual MaybeTime
    getModifyTime(const Info& info) = 0;

    /**
     * Reads the contents of a file
     *
     * @param file the file to read from
     * @param data the place to read the data to,
     *        if not enough data is left in file,
     *        it returns the number of bytes that could be read (including length of zero)
     * @return A slice to the bytes that could be read on success,
     *         Result on error
     */
    virtual MaybeSlice
    read(File& file, const outpost::Slice<uint8_t>& data) = 0;

    /**
     * Writes data to a file
     *
     * @param file the file to read from
     * @param data data to write to the file
     * @return Number of bytes that could be written on success,
     *         Result on error
     */
    virtual MaybeNumberOfBytes
    write(File& file, const outpost::Slice<const uint8_t>& data) = 0;

    /**
     * Moves the read/writer pointer of a file
     *
     * @param file filepointer for which the position should be changed
     * @param diff amount in bytes of which the pointer should be moved
     * @param mode position from which the pointer should be moved
     * @return Operation result
     */
    virtual Result
    seek(File& file, const filesystem::FileSizeDiff& diff, const SeekMode& mode) = 0;

    /**
     * Flushes a file
     *
     * @param file the file to flush
     * @return Operation result
     */
    virtual Result
    flush(File& file) = 0;

    /**
     * Flushes a filesystem
     * @return Operation result
     */
    virtual Result
    flush() = 0;

    /**
     * Sets the size of a file to a given value
     *
     * Note: implementation may support size reduction only
     * @param path absolute path to the file to truncate
     * @param newLength new length of the file
     * @return Operation result
     */
    virtual Result
    truncate(const BoundedString& path, const filesystem::FileSize& newLength) = 0;

    /**
     * Sets the size of a file to a given value
     *
     * Note: implementation may support size reduction only
     * @param entry the file to truncate
     * @param newLength new length of the file
     * @return Operation result
     */
    virtual Result
    truncate(DirectoryEntry& entry, const filesystem::FileSize& newLength) = 0;

    /**
     * Renames a element in the filesystem
     *
     * @param sourePath Absolute path of the object to rename
     * @param targetPath Absolute path which the element should have after operation
     * @return Operation result
     */
    virtual Result
    rename(const BoundedString& sourcePath, const BoundedString& targetPath) = 0;

    /**
     * Copies a element
     *
     * Note: implementation may support file copy only
     * @param sourePath Absolute path of the object to copy
     * @param targetPath Absolute path of the copied element
     * @return Operation result
     */
    virtual Result
    copy(const BoundedString& sourcePath, const BoundedString& targetPath) = 0;

    /**
     * Changes the permission of an element
     *
     * @param path absolute path of the element
     * @param perm the new permission
     * @return Operation result
     */
    virtual Result
    chmod(const BoundedString& path, const filesystem::Permission& perm) = 0;

    /**
     * Changes the permission of an element
     *
     * @param entry the elemt to change
     * @param perm the new permission
     * @return Operation result
     */
    virtual Result
    chmod(DirectoryEntry& entry, const filesystem::Permission& perm) = 0;

    /**
     * Removes an element
     *
     * Note: Implementation may require directories to be empty
     * @param path absolute path to the element to remove
     * @return Operation result
     */
    virtual Result
    remove(const BoundedString& path) = 0;

    /**
     * Removes an element
     *
     * Note: Implementation may require directories to be empty
     * @param parant the directory from which the entry should be removed
     * @param entry the element to remove
     * @return Operation result
     */
    virtual Result
    remove(Directory& parent, DirectoryEntry& entry) = 0;
};

}  // namespace hal
}  // namespace outpost

#endif
