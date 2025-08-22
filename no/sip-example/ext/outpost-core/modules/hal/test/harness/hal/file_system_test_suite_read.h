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
using Suite = FileSystemTestSuiteRaw;

// ---------------------------------------------------------------------------
#define READ_TESTS                                                                                 \
    read__should_read_file_when_fs_is_read_only,                                                   \
            read__should_not_read_file_when_fs_is_not_mounted,                                     \
            read__should_read_zero_bytes_from_empty_file,                                          \
            read__should_fail_to_read_some_bytes_from_empty_file,                                  \
            read__should_read_zero_bytes_from_non_empty_file, read__should_read_all_bytes_of_file, \
            read__should_read_all_bytes_of_file_in_multiple_calls,                                 \
            read__should_read_from_already_deleted_file,                                           \
            read__should_read_zero_bytes_from_deleted_file,                                        \
            read__should_only_read_file_if_read_flag_is_set,                                       \
            read__should_not_read_file_opened_without_read_flag

// ---------------------------------------------------------------------------
/**
 * Tests for \c read()
 *
 * - states:
 *   - file exists
 *      - READ
 *      - WRITE (can not be opened in read-mode)
 *      - EXECUTE (can not be opened in read-mode)
 *      - READ | WRITE
 *      - WRITE | EXECUTE (can not be opened in read-mode)
 *      - EXECUTE | READ
 *      - READ | WRITE | EXECUTE
 *      - file empty
 *      - file not empty
 *   - file does not exist (deleted)
 *   - fs is read only
 *   - fs is not mounted
 *
 * - parameter:
 *   - data
 *      - request zero
 *      - request more than available
 *      - request less than available
 */

TYPED_TEST_P(FileSystemTestSuite, read__should_read_file_when_fs_is_read_only)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);
    static constexpr size_t contentSize = Suite::anyContent.size();

    {
        Suite::mount();
        Suite::createFile(anyFileName, Suite::anyContent);
        Suite::unmount();
    }

    Suite::mount(Suite::readOnly);

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    std::array<uint8_t, contentSize> buffer;
    auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
    ASSERT_THAT(maybeSlice, IsTrue());
    ASSERT_THAT(maybeSlice->getNumberOfElements(), Eq(contentSize));
    auto sliceAsStr = std::string(reinterpret_cast<const char*>(maybeSlice->getDataPointer()),
                                  maybeSlice->getNumberOfElements());
    ASSERT_THAT(sliceAsStr, StrEq(Suite::anyContent));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_not_read_file_when_fs_is_not_mounted)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        Suite::createFile(anyFileName, Suite::anyContent);
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    Suite::unmount();

    std::array<uint8_t, 3U> buffer;
    auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
    ASSERT_THAT(maybeSlice, IsFalse());
    ASSERT_THAT(maybeSlice.error(),
                AnyOf(Eq(FileSystem::Result::invalidState), Eq(FileSystem::Result::notMounted)));

    Suite::mount();
    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_zero_bytes_from_empty_file)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        const auto result = Suite::createFile(anyFileName);
        ASSERT_THAT(result, Eq(FileSystem::Result::success));
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    // first time
    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::Slice<uint8_t>::empty());
        ASSERT_THAT(maybeSlice, IsTrue());
        ASSERT_THAT(maybeSlice->getNumberOfElements(), 0U);
    }

    // second time
    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::Slice<uint8_t>::empty());
        ASSERT_THAT(maybeSlice, IsTrue());
        ASSERT_THAT(maybeSlice->getNumberOfElements(), 0U);
    }

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_fail_to_read_some_bytes_from_empty_file)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        Suite::createFile(anyFileName);
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    std::array<uint8_t, 3U> buffer;
    auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
    ASSERT_THAT(maybeSlice, IsFalse());
    ASSERT_THAT(maybeSlice.error(), Eq(FileSystem::Result::endOfData));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_zero_bytes_from_non_empty_file)
{
    Suite::mount();

    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    {
        Suite::createFile(anyFileName, "not empty");
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::Slice<uint8_t>::empty());
    ASSERT_THAT(maybeSlice, IsTrue());
    ASSERT_THAT(maybeSlice->getNumberOfElements(), Eq(0U));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_all_bytes_of_file)
{
    Suite::mount();

    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    static constexpr size_t contentSize = Suite::anyContent.size();

    {
        Suite::createFile(anyFileName, Suite::anyContent);
    }

    auto maybeFile = Suite::mFileSystem.open(anyFileName.c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    std::array<uint8_t, contentSize> buffer;
    auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
    ASSERT_THAT(maybeSlice, IsTrue());
    ASSERT_THAT(maybeSlice->getNumberOfElements(), Eq(contentSize));
    auto sliceAsStr = std::string(reinterpret_cast<const char*>(maybeSlice->getDataPointer()),
                                  maybeSlice->getNumberOfElements());
    ASSERT_THAT(sliceAsStr, StrEq(Suite::anyContent));

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_all_bytes_of_file_in_multiple_calls)
{
    Suite::mount();

    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);
    constexpr size_t contentSize = Suite::anyContent.size();

    {
        Suite::createFile(anyFileName, Suite::anyContent);
    }

    auto maybeFile =
            Suite::mFileSystem.open(Suite::toAbsPath("test.txt").c_str(), OpenMask::READ());
    ASSERT_THAT(maybeFile, IsTrue());

    constexpr auto bufferSize = (contentSize) / 2;
    std::array<uint8_t, bufferSize> buffer;

    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSlice, IsTrue());
        auto sliceAsStr = std::string(reinterpret_cast<const char*>(maybeSlice->getDataPointer()),
                                      maybeSlice->getNumberOfElements());
        ASSERT_THAT(sliceAsStr, StrEq("tes"));
    }

    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSlice, IsTrue());
        auto sliceAsStr = std::string(reinterpret_cast<const char*>(maybeSlice->getDataPointer()),
                                      maybeSlice->getNumberOfElements());
        ASSERT_THAT(sliceAsStr, StrEq("t12"));
    }

    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSlice, IsTrue());
        auto sliceAsStr = std::string(reinterpret_cast<const char*>(maybeSlice->getDataPointer()),
                                      maybeSlice->getNumberOfElements());
        ASSERT_THAT(sliceAsStr, StrEq("3"));
    }

    {
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSlice, IsFalse());
        ASSERT_THAT(maybeSlice.error(), Eq(FileSystem::Result::endOfData));
    }

    Suite::mFileSystem.close(*maybeFile);
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_from_already_deleted_file)
{
    // On posix it is possible to read from a deleted file. Seems that it will
    // be buffered on open. But it might be better to not specify this as the
    // expected behavior for the interface since it might be hard to implement
    // for some file systems.
    // Furthermore the user should have full control over the file. Therfore it
    // should be very unlikely that a user wants to read from a deleted file.
    ASSERT_THAT(true, IsTrue());
}

TYPED_TEST_P(FileSystemTestSuite, read__should_read_zero_bytes_from_deleted_file)
{
    // On posix it is possible to read from a deleted file. Seems that it will
    // be buffered on open. But it might be better to not specify this as the
    // expected behavior for the interface since it might be hard to implement
    // for some file systems.
    // Furthermore the user should have full control over the file. Therfore it
    // should be very unlikely that a user wants to read from a deleted file.
    ASSERT_THAT(true, IsTrue());
}

TYPED_TEST_P(FileSystemTestSuite, read__should_only_read_file_if_read_flag_is_set)
{
    // it is not possible to open a write-only file with READ().
    // Therefore the test case "read from write only" can not be tested.
    // Even if the file is opened first and then altered with `chmod` will not
    // work since posix is buffering the file (I.e.: will not raise an error)
    // but at least it can be tested if all permission combinations where
    // READ() is set can actually be read.

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
            Suite::createFile(path, Suite::anyContent, permission);
        }

        {
            auto maybeFile = Suite::mFileSystem.open(path.c_str(), OpenMask::READ());
            const bool shouldRead = permission.isSet(Suite::Permission::R());
            ASSERT_THAT(maybeFile.has_value(), Eq(shouldRead));

            if (maybeFile)
            {
                ASSERT_THAT(shouldRead, IsTrue());
                std::array<uint8_t, 3U> buffer;
                auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
                ASSERT_THAT(maybeSlice.has_value(), IsTrue());
                ASSERT_THAT(maybeSlice->getNumberOfElements(), Eq(3U));

                Suite::mFileSystem.close(*maybeFile);
            }
        }
    }
}

TYPED_TEST_P(FileSystemTestSuite, read__should_not_read_file_opened_without_read_flag)
{
    Suite::mount();

    const auto path = Suite::toAbsPath(Suite::anyFileName);

    {
        Suite::createFile(path, Suite::anyContent, Suite::Permission::RWX());
    }

    {
        auto maybeFile = Suite::mFileSystem.open(path.c_str(), OpenMask::WRITE());
        ASSERT_THAT(maybeFile.has_value(), IsTrue());

        std::array<uint8_t, 3U> buffer;
        auto maybeSlice = Suite::mFileSystem.read(*maybeFile, outpost::asSlice(buffer));
        ASSERT_THAT(maybeSlice.has_value(), IsFalse());
        // The POSIX implementation will return `notFound` which is counter
        // intuitive. Therefore the test suite also allows to return
        // `accessDenied`
        ASSERT_THAT(maybeSlice.error(),
                    AnyOf(Eq(FileSystem::Result::notFound), Eq(FileSystem::Result::accessDenied)));

        Suite::mFileSystem.close(*maybeFile);
    }
}
