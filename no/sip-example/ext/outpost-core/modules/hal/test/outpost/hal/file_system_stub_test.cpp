/*
 * Copyright (c) 2020-2021, Jan Malburg
 * Copyright (c) 2023, Jan-Gerd Mess
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

#include <harness/hal/file_system.h>
#include <harness/hal/file_system_test_suite.h>

#include <outpost/time/time_epoch.h>

#include <unittest/hal/file_system_stub.h>
#include <unittest/harness.h>
#include <unittest/time/testing_clock.h>

using namespace harness::hal;
using outpost::asSlice;
using outpost::BoundedString;
using outpost::hal::FileSystem;
using namespace outpost::hal;
using namespace outpost::hal::filesystem;
using Result = outpost::hal::FileSystem::Result;
using File = outpost::hal::FileSystem::File;
using unittest::hal::FileSystemStub;
using unittest::time::TestingClock;

// ---------------------------------------------------------------------------
TEST(FileSystemStubTest, mounting)
{
    TestingClock clk;
    FileSystemStub fs(clk);

    EXPECT_EQ(false, fs.isMounted());
    EXPECT_EQ(Result::success, fs.mount(false));
    EXPECT_EQ(true, fs.isMounted());
    EXPECT_NE(Result::success, fs.mount(false));

    constexpr char dir1[] = "/a";
    EXPECT_EQ(Result::success, fs.mkDir(dir1, Permission::R() | Permission::W()));

    EXPECT_EQ(Result::success, fs.unmount());
    EXPECT_EQ(false, fs.isMounted());
    EXPECT_NE(Result::success, fs.unmount());

    EXPECT_EQ(Result::success, fs.mount(Suite::readOnly));
    EXPECT_EQ(true, fs.isMounted());

    constexpr char dir2[] = "/b";
    EXPECT_NE(Result::success, fs.mkDir(dir2, Permission::R() | Permission::W()));
}

TEST(FileSystemStubTest, forceError)
{
    TestingClock clk;
    FileSystemStub fs(clk);

    fs.forceError(Result::alreadyExists, 0);
    EXPECT_EQ(Result::alreadyExists, fs.mount(false));

    // no its works
    EXPECT_EQ(Result::success, fs.mount(false));

    // delayed
    fs.forceError(Result::alreadyExists, 2);
    constexpr char dir1[] = "/a";
    EXPECT_EQ(Result::success, fs.mkDir(dir1, Permission::R() | Permission::W()));

    constexpr char dir2[] = "/b";
    EXPECT_EQ(Result::success, fs.mkDir(dir2, Permission::R() | Permission::W()));

    constexpr char dir3[] = "/c";
    EXPECT_EQ(Result::alreadyExists, fs.mkDir(dir3, Permission::R() | Permission::W()));

    // now it works again
    EXPECT_EQ(Result::success, fs.mkDir(dir3, Permission::R() | Permission::W()));
}

TEST(FileSystemStubTest, readWriteSeek)
{
    TestingClock clk;
    FileSystemStub fs(clk);

    EXPECT_EQ(Result::success, fs.mount(false));

    constexpr char file[] = "/a";

    {
        auto mask = OpenMask::READ() | OpenMask::WRITE() | OpenMask::CREATE();
        auto maybeHandle = fs.open(file, mask);
        ASSERT_TRUE(maybeHandle);
        auto handle = *maybeHandle;

        constexpr uint8_t data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

        auto input = asSlice(data);

        auto maybeWrittenBytes = fs.write(handle, input);
        ASSERT_TRUE(maybeWrittenBytes);
        EXPECT_EQ(*maybeWrittenBytes, input.getNumberOfElements());

        uint8_t readBuffer[10];

        auto maybeReadData = fs.read(handle, asSlice(readBuffer));
        ASSERT_FALSE(maybeReadData);
        EXPECT_EQ(Result::endOfData, maybeReadData.error());

        EXPECT_EQ(Result::success, fs.seek(handle, -5, FileSystem::SeekMode::current));

        maybeReadData = fs.read(handle, asSlice(readBuffer));
        ASSERT_TRUE(maybeReadData);
        EXPECT_EQ(5u, maybeReadData->getNumberOfElements());
        EXPECT_TRUE(input.last(5).isContentEqual(*maybeReadData));

        EXPECT_EQ(Result::success, fs.seek(handle, 5, FileSystem::SeekMode::set));
        maybeWrittenBytes = fs.write(handle, input);
        ASSERT_TRUE(maybeWrittenBytes);
        EXPECT_EQ(*maybeWrittenBytes, input.getNumberOfElements());

        EXPECT_EQ(Result::success, fs.seek(handle, -15, FileSystem::SeekMode::end));

        uint8_t readBuffer2[15];
        auto maybeReadData2 = fs.read(handle, asSlice(readBuffer2));
        ASSERT_TRUE(maybeReadData2);
        EXPECT_EQ(15u, maybeReadData2->getNumberOfElements());

        EXPECT_TRUE(maybeReadData2->first(5).isContentEqual(input.first(5)));
        EXPECT_TRUE(maybeReadData2->skipFirst(5).isContentEqual(input));

        EXPECT_EQ(Result::success, fs.close(handle));
    }

    {
        auto mask = OpenMask::READ() | OpenMask::WRITE();
        auto maybeHandle = fs.open(file, mask);
        ASSERT_TRUE(maybeHandle);
        EXPECT_EQ(Result::success, fs.close(*maybeHandle));

        char file2[] = "/b";  // not existing file
        maybeHandle = fs.open(file2, mask);
        ASSERT_FALSE(maybeHandle);
        EXPECT_EQ(maybeHandle.error(), Result::notFound);

        EXPECT_EQ(Result::success, fs.unmount());
    }
}

TEST(FileSystemStubTest, mkDirCreateFileRenameCopyAndRemove)
{
    TestingClock clk;
    FileSystemStub fs(clk);

    EXPECT_EQ(Result::success, fs.mount(false));

    const BoundedString dirA{"/A"};
    const BoundedString dirB{"/B"};
    const BoundedString file{"/A/a"};

    const auto perm = Permission::RWX();

    EXPECT_EQ(Result::success, fs.mkDir(dirA, perm));
    EXPECT_EQ(Result::success, fs.mkDir(dirB, perm));
    EXPECT_EQ(Result::success, fs.createFile(file, perm));

    auto mask = OpenMask::READ() | OpenMask::WRITE();

    auto maybeHandle = fs.open(file, mask);
    ASSERT_TRUE(maybeHandle);
    File& handle = *maybeHandle;

    constexpr uint8_t data[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto maybeWrittenBytes = fs.write(handle, asSlice(data));
    ASSERT_TRUE(maybeWrittenBytes);
    EXPECT_EQ(*maybeWrittenBytes, asSlice(data).getNumberOfElements());

    EXPECT_EQ(Result::success, fs.close(handle));

    const BoundedString file2 = "/B/b";

    EXPECT_EQ(Result::success, fs.rename(file, file2));

    uint8_t readBuffer[10];

    // 1
    maybeHandle = fs.open(file2, mask);
    ASSERT_TRUE(maybeHandle);
    handle = *maybeHandle;
    auto maybeReadSlice = fs.read(handle, asSlice(readBuffer));
    ASSERT_TRUE(maybeReadSlice);
    EXPECT_EQ(10u, maybeReadSlice->getNumberOfElements());
    EXPECT_TRUE(asSlice(data).isContentEqual(*maybeReadSlice));
    EXPECT_EQ(Result::success, fs.close(handle));

    auto probablyNotYetFound = fs.open(file, mask);
    ASSERT_FALSE(probablyNotYetFound);
    EXPECT_EQ(probablyNotYetFound.error(), Result::notFound);
    EXPECT_EQ(Result::success, fs.copy(file2, file));

    // 2
    maybeHandle = fs.open(file2, mask);
    ASSERT_TRUE(maybeHandle);
    handle = *maybeHandle;
    maybeReadSlice = fs.read(handle, asSlice(readBuffer));
    ASSERT_TRUE(maybeReadSlice);
    EXPECT_EQ(10u, maybeReadSlice->getNumberOfElements());
    EXPECT_TRUE(asSlice(data).isContentEqual(*maybeReadSlice));
    EXPECT_EQ(Result::success, fs.close(handle));

    // 3
    maybeHandle = fs.open(file2, mask);
    ASSERT_TRUE(maybeHandle);
    handle = *maybeHandle;
    maybeReadSlice = fs.read(handle, asSlice(readBuffer));
    ASSERT_TRUE(maybeReadSlice);
    EXPECT_EQ(10u, maybeReadSlice->getNumberOfElements());
    EXPECT_TRUE(asSlice(data).isContentEqual(*maybeReadSlice));
    EXPECT_EQ(Result::success, fs.close(handle));

    EXPECT_EQ(Result::success, fs.remove(file));
    maybeHandle = fs.open(file, mask);
    ASSERT_FALSE(maybeHandle);
    EXPECT_EQ(maybeHandle.error(), Result::notFound);

    EXPECT_EQ(Result::success, fs.unmount());
}

TEST(FileSystemStubTest, openDirObjectInfoChmodAndTruncat)
{
    TestingClock clk;
    FileSystemStub fs(clk);
    auto time1 = outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                   outpost::time::GpsEpoch>::convert(clk.now());

    EXPECT_EQ(Result::success, fs.mount(false));

    constexpr char dirA[] = "/A";
    constexpr char dirB[] = "/B";
    constexpr char dirC[] = "/C";
    constexpr char file1[] = "/1";
    constexpr char file2[] = "/2";
    constexpr char file3[] = "/3";
    constexpr char subFile1[] = "/B/x";
    constexpr char subFile2[] = "/B/a";
    constexpr FileSize sizeFile1 = 10;
    FileSize sizeFile2 = 5;

    Permission perm = Permission::R() | Permission::W() | Permission::X();
    EXPECT_EQ(perm, Permission::RWX());
    EXPECT_TRUE(perm.isReadable());
    EXPECT_TRUE(perm.isWritable());
    EXPECT_TRUE(perm.isExecutable());

    EXPECT_EQ(Result::success, fs.mkDir(dirA, perm));
    EXPECT_EQ(Result::success, fs.mkDir(dirB, perm));
    EXPECT_EQ(Result::success, fs.createFile(file1, perm));
    EXPECT_EQ(Result::success, fs.createFile(file2, perm));

    auto mask = OpenMask::READ() | OpenMask::WRITE();
    auto maybeHandle = fs.open(file1, mask);
    ASSERT_TRUE(maybeHandle);
    File& handle = *maybeHandle;

    const uint8_t data[sizeFile1] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    auto maybeWrittenBytes = fs.write(handle, asSlice(data));
    ASSERT_TRUE(maybeWrittenBytes);
    EXPECT_EQ(*maybeWrittenBytes, sizeFile1);
    EXPECT_EQ(Result::success, fs.close(handle));

    clk.incrementBy(outpost::time::Seconds(1));
    auto time2 = outpost::time::TimeEpochConverter<outpost::time::SpacecraftElapsedTimeEpoch,
                                                   outpost::time::GpsEpoch>::convert(clk.now());

    // modify file after creating
    maybeHandle = fs.open(file2, mask);
    EXPECT_TRUE(maybeHandle);
    handle = *maybeHandle;
    maybeWrittenBytes = fs.write(handle, asSlice(data));
    ASSERT_TRUE(maybeWrittenBytes);
    EXPECT_EQ(*maybeWrittenBytes, asSlice(data).getNumberOfElements());
    EXPECT_EQ(Result::success, fs.close(handle));

    EXPECT_EQ(Result::success, fs.truncate(file2, sizeFile2));
    auto perm2 = Permission::R();
    EXPECT_EQ(Result::success, fs.chmod(file2, perm2));  // correct behavior checked below

    // modify dir after creating, also this setup the test of remove later
    EXPECT_EQ(Result::success, fs.createFile(subFile1, perm));
    EXPECT_EQ(Result::success, fs.mkDir(subFile2, perm));
    EXPECT_EQ(Result::success, fs.remove(subFile2));

    EXPECT_EQ(Result::success, fs.mkDir(dirC, perm));
    EXPECT_EQ(Result::success, fs.createFile(file3, perm));

    {
        auto maybeInfo = fs.getInfo(dirB);
        ASSERT_TRUE(maybeInfo);
        FileSystem::Info& info = *maybeInfo;

        const auto maybeFile = fs.isFile(info);
        ASSERT_TRUE(maybeFile.has_value());
        EXPECT_EQ(*maybeFile, false);

        const auto maybeDirectory = fs.isDirectory(info);
        ASSERT_TRUE(maybeDirectory.has_value());
        EXPECT_EQ(*maybeDirectory, true);

        const auto maybePermissions = fs.getPermissions(info);
        ASSERT_TRUE(maybePermissions);
        EXPECT_EQ(*maybePermissions, perm);

        const auto maybeCreationTime = fs.getCreationTime(info);
        ASSERT_TRUE(maybeCreationTime);
        EXPECT_EQ(*maybeCreationTime, time1);

        const auto maybeModifyTime = fs.getModifyTime(info);
        ASSERT_TRUE(maybeModifyTime);
        EXPECT_EQ(*maybeModifyTime, time2);

        const auto maybeSize = fs.getSize(info);
        ASSERT_FALSE(maybeSize);
        EXPECT_EQ(maybeSize.error(), Result::notAFile);

        EXPECT_EQ(Result::success, fs.releaseInfo(info));
    }

    {
        auto maybeInfo = fs.getInfo(file2);
        ASSERT_TRUE(maybeInfo);
        FileSystem::Info& info = *maybeInfo;

        const auto maybeFile = fs.isFile(info);
        ASSERT_TRUE(maybeFile.has_value());
        EXPECT_EQ(*maybeFile, true);

        const auto maybeDirectory = fs.isDirectory(info);
        ASSERT_TRUE(maybeDirectory.has_value());
        EXPECT_EQ(*maybeDirectory, false);

        const auto maybePermissions = fs.getPermissions(info);
        ASSERT_TRUE(maybePermissions);
        EXPECT_EQ(*maybePermissions, perm2);

        const auto maybeCreationTime = fs.getCreationTime(info);
        ASSERT_TRUE(maybeCreationTime);
        EXPECT_EQ(*maybeCreationTime, time1);

        const auto maybeModifyTime = fs.getModifyTime(info);
        ASSERT_TRUE(maybeModifyTime);
        EXPECT_EQ(*maybeModifyTime, time2);

        const auto maybeSize = fs.getSize(info);
        ASSERT_TRUE(maybeSize);
        EXPECT_EQ(*maybeSize, sizeFile2);

        EXPECT_EQ(Result::success, fs.releaseInfo(info));
    }

    const BoundedString root = "/";
    auto maybeDir = fs.openDir(root);
    ASSERT_TRUE(maybeDir);
    FileSystem::Directory& dir = *maybeDir;

    PathBuffer nameBuf;
    bool found[6] = {false, false, false, false, false};
    for (int i = 0; i < 6; i++)
    {
        auto maybeDirEntry = fs.readDir(dir);
        ASSERT_TRUE(maybeDirEntry);
        FileSystem::DirectoryEntry& dirEntry = *maybeDirEntry;
        const auto maybeName = fs.getName(dirEntry, asSlice(nameBuf));
        ASSERT_TRUE(maybeName);
        EXPECT_EQ(1u, maybeName->size());

        auto maybeInfo = fs.getInfo(dirEntry);
        ASSERT_TRUE(maybeInfo);
        FileSystem::Info& info = *maybeInfo;
        const auto maybeFile = fs.isFile(info);
        ASSERT_TRUE(maybeFile.has_value());
        const auto maybeDirectory = fs.isDirectory(info);
        ASSERT_TRUE(maybeDirectory.has_value());
        const auto maybePermissions = fs.getPermissions(info);
        ASSERT_TRUE(maybePermissions);
        const auto maybeCreationTime = fs.getCreationTime(info);
        ASSERT_TRUE(maybeCreationTime);
        const auto maybeModifyTime = fs.getModifyTime(info);
        ASSERT_TRUE(maybeModifyTime);
        const auto maybeSize = fs.getSize(info);
        // no check for success yet

        EXPECT_EQ(Result::success, fs.releaseInfo(info));
        switch ((*maybeName)[0])
        {
            case 'A':
                found[0] = true;
                EXPECT_EQ(*maybeFile, false);
                EXPECT_EQ(*maybeDirectory, true);
                EXPECT_EQ(*maybePermissions, perm);
                EXPECT_EQ(*maybeCreationTime, time1);
                EXPECT_EQ(*maybeModifyTime, time1);
                EXPECT_FALSE(maybeSize);
                EXPECT_EQ(maybeSize.error(), Result::notAFile);
                break;
            case 'B':
                found[1] = true;
                EXPECT_EQ(*maybeFile, false);
                EXPECT_EQ(*maybeDirectory, true);
                EXPECT_EQ(*maybePermissions, perm);
                EXPECT_EQ(*maybeCreationTime, time1);
                EXPECT_EQ(*maybeModifyTime, time2);
                EXPECT_FALSE(maybeSize);
                EXPECT_EQ(maybeSize.error(), Result::notAFile);

                // test openDir with Direntry
                {
                    auto maybeSubDir = fs.openDir(dirEntry);
                    ASSERT_TRUE(maybeSubDir);

                    auto maybeSubEntry = fs.readDir(*maybeSubDir);
                    ASSERT_TRUE(maybeSubEntry);

                    PathBuffer name2Buf;
                    auto maybeSubEntryName = fs.getName(*maybeSubEntry, asSlice(name2Buf));
                    ASSERT_TRUE(maybeSubEntryName);
                    EXPECT_TRUE(maybeSubEntryName->isContentEqual(BoundedString("x")));

                    // Test remove with Direnty
                    EXPECT_EQ(Result::success, fs.remove(*maybeSubDir, *maybeSubEntry));
                    EXPECT_EQ(Result::success, fs.closeDir(*maybeSubDir));

                    maybeSubDir = fs.openDir(dirEntry);
                    ASSERT_TRUE(maybeSubDir);
                    // this is the test that both removes were successful
                    maybeSubEntry = fs.readDir(*maybeSubDir);
                    ASSERT_FALSE(maybeSubEntry);
                    EXPECT_EQ(maybeSubEntry.error(), Result::endOfData);
                    EXPECT_EQ(Result::success, fs.closeDir(*maybeSubDir));
                }

                break;
            case 'C':
                found[2] = true;
                EXPECT_EQ(*maybeFile, false);
                EXPECT_EQ(*maybeDirectory, true);
                EXPECT_EQ(*maybePermissions, perm);
                EXPECT_EQ(*maybeCreationTime, time2);
                EXPECT_EQ(*maybeModifyTime, time2);
                EXPECT_FALSE(maybeSize);
                EXPECT_EQ(maybeSize.error(), Result::notAFile);
                break;
            case '1':
                found[3] = true;
                EXPECT_EQ(*maybeFile, true);
                EXPECT_EQ(*maybeDirectory, false);
                EXPECT_EQ(*maybePermissions, perm);
                EXPECT_EQ(*maybeCreationTime, time1);
                EXPECT_EQ(*maybeModifyTime, time1);
                ASSERT_TRUE(maybeSize);
                EXPECT_EQ(*maybeSize, sizeFile1);
                break;
            case '2':
                found[4] = true;
                EXPECT_EQ(*maybeFile, true);
                EXPECT_EQ(*maybeDirectory, false);
                EXPECT_EQ(*maybePermissions, perm2);
                EXPECT_EQ(*maybeCreationTime, time1);
                EXPECT_EQ(*maybeCreationTime, time1);
                EXPECT_EQ(*maybeModifyTime, time2);
                ASSERT_TRUE(maybeSize);
                EXPECT_EQ(*maybeSize, sizeFile2);
                break;
            case '3':
                found[5] = true;

                // open with direnty
                maybeHandle = fs.open(dirEntry, mask);
                ASSERT_TRUE(maybeHandle);
                maybeWrittenBytes = fs.write(*maybeHandle, asSlice(data));
                ASSERT_TRUE(maybeWrittenBytes);
                EXPECT_EQ(*maybeWrittenBytes, asSlice(data).getNumberOfElements());
                EXPECT_EQ(Result::success, fs.close(*maybeHandle));

                maybeInfo = fs.getInfo(dirEntry);
                ASSERT_TRUE(maybeInfo);
                {
                    FileSystem::Info& info2 = *maybeInfo;
                    const auto maybeFile2 = fs.isFile(info2);
                    ASSERT_TRUE(maybeFile2.has_value());
                    const auto maybeDirectory2 = fs.isDirectory(info);
                    ASSERT_TRUE(maybeDirectory2.has_value());
                    const auto maybePermissions2 = fs.getPermissions(info);
                    ASSERT_TRUE(maybePermissions2);
                    const auto maybeCreationTime2 = fs.getCreationTime(info);
                    ASSERT_TRUE(maybeCreationTime2);
                    const auto maybeModifyTime2 = fs.getModifyTime(info);
                    ASSERT_TRUE(maybeModifyTime2);
                    const auto maybeSize2 = fs.getSize(info);
                    ASSERT_TRUE(*maybeSize2);

                    EXPECT_EQ(*maybeFile2, true);
                    EXPECT_EQ(*maybeDirectory2, false);
                    EXPECT_EQ(*maybePermissions2, perm);
                    EXPECT_EQ(*maybeCreationTime2, time2);
                    EXPECT_EQ(*maybeModifyTime2, time2);
                    EXPECT_EQ(*maybeSize2,
                              sizeFile1);  // this checks also that the write was succesful
                }

                EXPECT_EQ(Result::success, fs.releaseInfo(*maybeInfo));
                break;
            default: EXPECT_TRUE(false); break;
        }
    }
    EXPECT_TRUE(found[0] && found[1] && found[2] && found[3] && found[4] && found[5]);

    auto probablyNoEntry = fs.readDir(dir);
    ASSERT_FALSE(probablyNoEntry);
    EXPECT_EQ(probablyNoEntry.error(), Result::endOfData);

    EXPECT_EQ(Result::success, fs.rewindDir(dir));

    auto probablyAnEntry = fs.readDir(dir);
    ASSERT_TRUE(probablyAnEntry);
    EXPECT_EQ(Result::success, fs.closeDir(dir));

    EXPECT_EQ(Result::success, fs.unmount());
}

// ---------------------------------------------------------------------------
/**
 * \class TestHelper
 */
class TestHelper : public harness::hal::TestHelper
{
public:
    virtual ~TestHelper() = default;

    std::string
    toAbsPath(const std::string& path) const override
    {
        return "/" + path;
    }

    void
    clearWorkspace() override
    {
        // nothing to do, since every test case will get its own instance
        // of the stub.
    }

private:
};

// ---------------------------------------------------------------------------
/**
 * \class FileSystemStubWrapper
 */
class FileSystemStubWrapper : public unittest::hal::FileSystemStub
{
public:
    using TestHelper = ::TestHelper;

    explicit FileSystemStubWrapper() : unittest::hal::FileSystemStub(mClock)
    {
    }

private:
    unittest::time::TestingClock mClock;
};

// ---------------------------------------------------------------------------
using FileSystemTypes = testing::Types<FileSystemStubWrapper>;
INSTANTIATE_TYPED_TEST_SUITE_P(FileSystemStubTest,
                               FileSystemTestSuite,
                               FileSystemTypes,
                               NamePrinter);
