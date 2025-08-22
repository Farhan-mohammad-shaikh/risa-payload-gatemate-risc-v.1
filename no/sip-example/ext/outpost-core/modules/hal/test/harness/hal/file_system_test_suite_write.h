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

#include "file_system_test_suite.h"

// its a bad practice to use `using` in header files, but since the header is
// only include into source files it should be ok.
using namespace ::testing;

using outpost::hal::FileSystem;
using OpenMask = outpost::hal::filesystem::OpenMask;
using Suite = harness::hal::FileSystemTestSuiteRaw;

// ---------------------------------------------------------------------------
#define WRITE_TESTS                                                                                \
    write__should_not_write_file_when_fs_is_read_only,                                             \
            write__should_not_write_file_when_fs_is_not_mounted, write__should_write_data_to_file, \
            write__should_overwrite_file_content, write__should_write_zero_bytes,                  \
            write__should_write_to_already_deleted_file,                                           \
            write__should_write_zero_bytes_to_already_deleted_file,                                \
            write__should_only_write_file_if_write_flag_is_set,                                    \
            write__should_not_write_file_opened_without_write_flag

// ---------------------------------------------------------------------------
/**
 * Tests for \c write
 *
 * - states:
 *   - file exists
 *      - READ (can not be opened for writing)
 *      - WRITE
 *      - EXECUTE (can not be opened for writing)
 *      - READ | WRITE
 *      - WRITE | EXECUTE
 *      - EXECUTE | READ (can not be opened for writing)
 *      - READ | WRITE | EXECUTE
 *      - file empty
 *      - file not empty
 *   - file does not exist (deleted)
 *   - fs is read only
 *   - fs is not mounted
 *
 * - parameter:
 *   - data
 *      - empty
 *      - not empty
 */

TYPED_TEST_P(FileSystemTestSuite, write__should_not_write_file_when_fs_is_read_only)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    {
        Suite::mount();
        Suite::createFile(anyFileName, Suite::anyContent);
        Suite::unmount();
    }

    Suite::mount(Suite::readOnly);

    // Can not be opened in WRITE mode since file system is read only
    // (would already fail) -> but just for testing, open it in READ mode instead
    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    const auto maybeSize = Suite::mFileSystem.write(
            *maybeFile, outpost::BoundedString(Suite::anyContent).asUint8Slice());
    ASSERT_THAT(maybeSize, IsFalse());
    ASSERT_THAT(maybeSize.error(), Eq(outpost::hal::FileSystem::Result::readOnly));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, write__should_not_write_file_when_fs_is_not_mounted)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        Suite::createFile(anyFileName, Suite::anyContent);
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::WRITE());
    ASSERT_THAT(maybeFile, IsTrue());

    Suite::unmount();

    std::array<uint8_t, 3U> buffer = {'1', '2', '3'};
    auto maybeSlice = Suite::mFileSystem.write(*maybeFile, outpost::asSlice(buffer));
    ASSERT_THAT(maybeSlice, IsFalse());
    ASSERT_THAT(maybeSlice.error(),
                AnyOf(Eq(FileSystem::Result::invalidState), Eq(FileSystem::Result::notMounted)));

    Suite::mount();
    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, write__should_write_data_to_file)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);
    Suite::mount();

    auto maybeFile =
            Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::WRITE() | OpenMask::CREATE());
    ASSERT_THAT(maybeFile, IsTrue());

    {
        std::array<uint8_t, 3U> buffer = {'1', '2', '3'};
        auto maybeSize = Suite::mFileSystem.write(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSize, IsTrue());
        ASSERT_THAT(*maybeSize, Eq(3U));
    }

    Suite::checkFileContent(anyFileName, "123");
    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, write__should_overwrite_file_content)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        Suite::createFile(anyFileName, "Hello, World!");
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::WRITE());
    ASSERT_THAT(maybeFile, IsTrue());

    {
        std::array<uint8_t, 3U> buffer = {'1', '2', '3'};
        auto maybeSize = Suite::mFileSystem.write(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSize, IsTrue());
        ASSERT_THAT(*maybeSize, Eq(3U));
    }

    Suite::checkFileContent(anyFileName, "123lo, World!");
    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, write__should_write_zero_bytes)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    auto maybeFile =
            Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::WRITE() | OpenMask::CREATE());
    ASSERT_THAT(maybeFile, IsTrue());

    auto maybeSize = Suite::mFileSystem.write(*maybeFile, outpost::emptySlice());
    ASSERT_THAT(maybeSize, IsTrue());
    ASSERT_THAT(*maybeSize, Eq(0U));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, write__should_write_to_already_deleted_file)
{
    // On posix it is possible to write to a deleted file. Seems that it will
    // be buffered on open. But it might be better to not specify this as the
    // expected behavior for the interface since it might be hard to implement
    // for some file systems.
    // Furthermore the user should have full control over the file. Therfore it
    // should be very unlikely that a user wants to write to a deleted file.
}

TYPED_TEST_P(FileSystemTestSuite, write__should_write_zero_bytes_to_already_deleted_file)
{
    // On posix it is possible to write to a deleted file. Seems that it will
    // be buffered on open. But it might be better to not specify this as the
    // expected behavior for the interface since it might be hard to implement
    // for some file systems.
    // Furthermore the user should have full control over the file. Therfore it
    // should be very unlikely that a user wants to write to a deleted file.
}

TYPED_TEST_P(FileSystemTestSuite, write__should_only_write_file_if_write_flag_is_set)
{
    // it is not possible to open a read-only file with WRITE().
    // Therefore the test case "write to read only" can not be tested.
    // Even if the file is opened first and then altered with `chmod` will not
    // work since posix is buffering the file (I.e.: will not raise an error)
    // but at least it can be tested if all permission combinations where
    // WRITE() is set can actually be written.

    Suite::mount();

    for (const auto permission : Suite::allPermissions)
    {
        std::string permissionAsString = ::testing::PrintToString(permission);
        // ensure that the correct `PrintTo()` method is used. Might be that a
        // violation to ODR breaks this. In this case the name would be a hex
        // dump of the permission object.
        ASSERT_THAT(permissionAsString.size(), Le(3U)) << "Invalid test setup; ensure that "
                                                          "PrintTo() is available";

        const auto fileName = "test_" + permissionAsString + ".txt";
        const auto path = Suite::toAbsPath(fileName);

        {
            Suite::createFile(path, "test 123", permission);
        }

        {
            auto maybeFile = Suite::mFileSystem.open(path.c_str(), OpenMask::WRITE());
            const bool shouldWrite = permission.isSet(Suite::Permission::W());
            ASSERT_THAT(maybeFile.has_value(), Eq(shouldWrite));

            if (maybeFile)
            {
                ASSERT_THAT(shouldWrite, IsTrue());
                std::array<const uint8_t, 3U> buffer = {'1', '2', '3'};
                auto maybeSize = Suite::mFileSystem.write(*maybeFile, outpost::asSlice(buffer));
                ASSERT_THAT(maybeSize.has_value(), IsTrue());
                ASSERT_THAT(*maybeSize, Eq(3U));

                Suite::mFileSystem.close(*maybeFile);
            }
        }
    }
}

TYPED_TEST_P(FileSystemTestSuite, write__should_not_write_file_opened_without_write_flag)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);
    Suite::mount();

    {
        Suite::createFile(anyFileName, Suite::anyContent, Suite::Permission::RWX());
    }

    {
        auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
        ASSERT_THAT(maybeFile.has_value(), IsTrue());

        std::array<const uint8_t, 3U> buffer = {'1', '2', '3'};
        auto maybeSize = Suite::mFileSystem.write(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSize.has_value(), IsFalse());
        // The POSIX implementation will return `notFound` which is counter
        // intuitive. Therefore the test suite also allows to return
        // `accessDenied`
        ASSERT_THAT(maybeSize.error(),
                    AnyOf(Eq(FileSystem::Result::notFound), Eq(FileSystem::Result::accessDenied)));

        Suite::mFileSystem.close(*maybeFile);
    }
}
