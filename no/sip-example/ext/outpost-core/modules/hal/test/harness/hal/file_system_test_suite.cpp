/*
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "harness/hal/file_system_test_suite.h"

using namespace outpost::hal;
using namespace outpost::hal::filesystem;
using namespace harness::hal;
using namespace ::testing;

// ---------------------------------------------------------------------------

/**
 * Base class with functions that don't yet need the templated class of the actual filesystem ...
 * bulub blab
 */
FileSystemTestSuiteRaw::FileSystemTestSuiteRaw(FileSystem& fs, TestHelper& helper) :
    mFileSystem(fs), mHelper(helper)
{
}

void
FileSystemTestSuiteRaw::SetUp()
{
    const auto invalidPath = outpost::BoundedString("");
    // TODO (rein_j1) could be const
    auto invalidFile = FileSystem::File{0};
    // TODO (rein_j1) could be const
    auto invalidInfo = FileSystem::Info{0};

    // check that all methods are implemented which are needed to execute this
    // test suite. All methods are called with invalid arguments. If the method
    // is yet not implement it will return `notImplemented`. If it is
    // implemented it should be immediately return with `invalidArgument`.

    unsigned numRequirementsMet = 0;
    constexpr int numberOfMethodsRequired = 11;

    // mount and unmount do not have parameters so it is not possible to force
    // an error. But after mount and unmount the file system should be in the
    // initial state again.
    numRequirementsMet += mFileSystem.mount(readOnly) != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.unmount() != FileSystem::Result::notImplemented;

    numRequirementsMet += mFileSystem.createFile(invalidPath, Permission::R())
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.open(invalidPath, OpenMask::APPEND()).error()
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.close(invalidFile) != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.write(invalidFile, outpost::emptySlice()).error()
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.read(invalidFile, outpost::emptySlice()).error()
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.mkDir(invalidPath) != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.getInfo(invalidPath).error()
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.releaseInfo(invalidInfo)
                          != FileSystem::Result::notImplemented;
    numRequirementsMet += mFileSystem.getPermissions(invalidInfo).error()
                          != FileSystem::Result::notImplemented;

    if (numRequirementsMet != numberOfMethodsRequired)
    {
        GTEST_SKIP() << "`FileSystem` minimal implementation not met! (" << numRequirementsMet
                     << "/" << numberOfMethodsRequired << ")";
    }

    mHelper.clearWorkspace();
}

void
FileSystemTestSuiteRaw::TearDown()
{
    mHelper.clearWorkspace();
}

// ---------------------------------------------------------------------------
void
FileSystemTestSuiteRaw::mount(bool mountReadOnly)
{
    const auto result = mFileSystem.mount(mountReadOnly);
    EXPECT_THAT(result, Eq(FileSystem::Result::success));
}

void
FileSystemTestSuiteRaw::unmount()
{
    const auto result = mFileSystem.unmount();
    EXPECT_THAT(result, Eq(FileSystem::Result::success));
}

FileSystem::Result
FileSystemTestSuiteRaw::createFile(const std::string& path)
{
    return mFileSystem.createFile(path.c_str(), Permission::RWX());
}

outpost::hal::FileSystem::Result
FileSystemTestSuiteRaw::createFile(const std::string& path, const Permission& permission)
{
    return mFileSystem.createFile(path.c_str(), permission);
}

void
FileSystemTestSuiteRaw::createFile(const std::string& path, const std::string_view& content)
{
    createFile(path, content, Permission::RWX());
}

void
FileSystemTestSuiteRaw::createFile(const std::string& path,
                                   const std::string_view& content,
                                   const Permission& permission)
{
    {
        const auto result = mFileSystem.createFile(path.c_str(), Permission::RWX());
        EXPECT_THAT(result, Eq(FileSystem::Result::success));
    }

    auto maybeFile = mFileSystem.open(path.c_str(), OpenMask::WRITE());
    EXPECT_THAT(maybeFile, IsTrue());

    if (maybeFile)
    {
        auto contentAsUint8 = outpost::BoundedString(content).asUint8Slice();

        while (contentAsUint8.getNumberOfElements() > 0U)
        {
            const auto maybeSize = mFileSystem.write(*maybeFile, contentAsUint8);
            EXPECT_THAT(maybeSize, IsTrue());

            if (maybeSize)
            {
                contentAsUint8 = contentAsUint8.skipFirst(*maybeSize);
            }
            else
            {
                contentAsUint8 = outpost::emptySlice();
            }
        }

        const auto result = mFileSystem.close(*maybeFile);
        EXPECT_THAT(result, Eq(FileSystem::Result::success));
    }

    {
        const auto result = mFileSystem.chmod(path.c_str(), permission);
        EXPECT_THAT(result, Eq(FileSystem::Result::success));
    }
}

outpost::hal::FileSystem::Result
FileSystemTestSuiteRaw::mkDir(const std::string& path, const Permission& permission)
{
    return mFileSystem.mkDir(path.c_str(), permission);
}

void
FileSystemTestSuiteRaw::checkFileExists(const std::string& path)
{
    auto maybeFile = mFileSystem.open(path.c_str(), OpenMask::READ());
    EXPECT_THAT(maybeFile, IsTrue());

    if (maybeFile)
    {
        const auto result = mFileSystem.close(*maybeFile);
        EXPECT_THAT(result, Eq(FileSystem::Result::success));
    }
}

void
FileSystemTestSuiteRaw::checkFileContent(const std::string& path, const std::string_view& content)
{
    auto maybeFile = mFileSystem.open(path.c_str(), OpenMask::READ());
    EXPECT_THAT(maybeFile, IsTrue());

    std::vector<char> fileContent;

    if (maybeFile)
    {
        bool again = true;
        while (again)
        {
            std::array<uint8_t, 1> buffer;
            const auto maybeSlice = mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
            if (maybeSlice && maybeSlice->getNumberOfElements() == 1U)
            {
                fileContent.push_back(buffer[0]);
            }
            else
            {
                again = false;
            }
        }

        const auto result = mFileSystem.close(*maybeFile);
        EXPECT_THAT(result, Eq(FileSystem::Result::success));

        fileContent.push_back('\0');
        EXPECT_THAT(&fileContent.at(0), StrEq(content));
    }
}

void
FileSystemTestSuiteRaw::checkPermission(const std::string& path, const Permission& permission)
{
    auto maybeInfo = mFileSystem.getInfo(path.c_str());
    EXPECT_THAT(maybeInfo, IsTrue());

    if (maybeInfo)
    {
        auto maybePermission = mFileSystem.getPermissions(*maybeInfo);
        EXPECT_THAT(maybePermission, IsTrue());

        if (maybePermission)
        {
            EXPECT_THAT(*maybePermission, Eq(permission));
        }

        mFileSystem.releaseInfo(*maybeInfo);
    }
}

std::string
FileSystemTestSuiteRaw::toAbsPath(const std::string& path) const
{
    return mHelper.toAbsPath(path);
}

std::string
FileSystemTestSuiteRaw::createTooLongPath() const
{
    const auto basePath = mHelper.toAbsPath("");
    const auto path = basePath + std::string(maxPathLength - basePath.size() + 1U, 'a');
    assert(path.size() == maxPathLength + 1);
    return path;
}
