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
using Suite = FileSystemTestSuiteRaw;

// ---------------------------------------------------------------------------
#define CREATE_FILE_TESTS                                                                 \
    createFile__should_create_file, createFile__should_not_create_file_if_already_exists, \
            createFile__should_not_create_file_if_parent_folder_does_not_exist,           \
            createFile__should_not_create_file_if_folder_with_same_name_exist,            \
            createFile__should_not_create_file_when_fs_is_read_only,                      \
            createFile__should_not_create_file_when_fs_is_not_mounted,                    \
            createFile__should_not_create_file_when_parent_is_read_only,                  \
            createFile__should_create_file_with_expected_permission,                      \
            createFile__should_reject_too_long_path, createFile__should_reject_empty_path

// ---------------------------------------------------------------------------
/**
 * Tests for \c createFile()
 *
 * - states:
 *   - directory already exists
 *   - file already exists
 *   - does not exist
 *   - parent folder does not exist
 *   - parent folder is read-only
 *   - fs is read only
 *   - fs is not mounted
 *
 * - parameter:
 *   - path:
 *      - valid
 *      - path exceeds max path length
 *      - empty
 *   - permission:
 *      - READ
 *      - WRITE
 *      - EXECUTE
 *      - READ | WRITE
 *      - WRITE | EXECUTE
 *      - EXECUTE | READ
 *      - READ | WRITE | EXECUTE
 */

// ---------------------------------------------------------------------------
TYPED_TEST_P(FileSystemTestSuite, createFile__should_create_file)
{
    const auto anyFileName = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    auto result = Suite::createFile(anyFileName);
    ASSERT_THAT(result, Eq(FileSystem::Result::success));

    Suite::checkFileExists(anyFileName);
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_not_create_file_if_already_exists)
{
    const auto anyFile = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        const auto result = Suite::createFile(anyFile);
        ASSERT_THAT(result, Eq(FileSystem::Result::success));
    }

    {
        const auto result = Suite::createFile(anyFile);
        ASSERT_THAT(result, Eq(FileSystem::Result::alreadyExists));
    }
}

TYPED_TEST_P(FileSystemTestSuite,
             createFile__should_not_create_file_if_parent_folder_does_not_exist)
{
    const auto anyFile = Suite::toAbsPath(std::string("test/") + Suite::anyFileName);

    Suite::mount();

    const auto result = Suite::createFile(anyFile);
    ASSERT_THAT(result, Eq(FileSystem::Result::notFound));
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_not_create_file_if_folder_with_same_name_exist)
{
    const auto anyFile = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount();

    {
        const auto result = Suite::mkDir(anyFile);
        ASSERT_THAT(result, Eq(FileSystem::Result::success));
    }

    {
        const auto result = Suite::createFile(anyFile);
        ASSERT_THAT(result, Eq(FileSystem::Result::alreadyExists));
    }
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_not_create_file_when_fs_is_read_only)
{
    const auto anyFile = Suite::toAbsPath(Suite::anyFileName);

    Suite::mount(Suite::readOnly);

    auto result = Suite::createFile(anyFile);
    ASSERT_THAT(result, Eq(FileSystem::Result::readOnly));
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_not_create_file_when_fs_is_not_mounted)
{
    const auto anyFile = Suite::toAbsPath(Suite::anyFileName);

    auto result = Suite::createFile(anyFile);
    ASSERT_THAT(result,
                AnyOf(Eq(FileSystem::Result::invalidState), Eq(FileSystem::Result::notMounted)));
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_not_create_file_when_parent_is_read_only)
{
    const auto parentFolder = Suite::toAbsPath("test");
    const auto anyFile = Suite::toAbsPath(std::string("test/") + Suite::anyFileName);

    Suite::mount();

    {
        const auto result = Suite::mkDir(parentFolder, Suite::Permission::R());
        ASSERT_THAT(result, Eq(FileSystem::Result::success));
    }

    auto result = Suite::createFile(anyFile);
    ASSERT_THAT(result, Eq(FileSystem::Result::accessDenied));
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_create_file_with_expected_permission)
{
    Suite::mount();

    for (const auto& permission : Suite::allPermissions)
    {
        const auto permissionAsString = ::testing::PrintToString(permission);

        // ensure that the correct `PrintTo()` method is used. Might be that a
        // violation to ODR breaks this. In this case the name would be a hex
        // dump of the permission object.
        ASSERT_THAT(permissionAsString.size(), Le(3U)) << "Invalid test setup; ensure that "
                                                          "PrintTo() is available";

        const auto fileName = "test_" + permissionAsString + ".txt";
        const auto path = Suite::toAbsPath(fileName);

        const auto result = Suite::createFile(path, permission);
        ASSERT_THAT(result, Eq(FileSystem::Result::success));

        Suite::checkPermission(path, permission);
    }
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_reject_too_long_path)
{
    const auto tooLongPath = Suite::createTooLongPath();

    Suite::mount();
    const auto result = Suite::createFile(tooLongPath);

    ASSERT_THAT(result, Eq(FileSystem::Result::invalidInput));
}

TYPED_TEST_P(FileSystemTestSuite, createFile__should_reject_empty_path)
{
    const auto emptyPath = std::string("");

    Suite::mount();
    const auto result = Suite::createFile(emptyPath);

    ASSERT_THAT(result, Eq(FileSystem::Result::invalidInput));
}
