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

#ifndef HARNESS_HAL_FILE_SYSTEM_TEST_SUITE_H
#define HARNESS_HAL_FILE_SYSTEM_TEST_SUITE_H

#include <cxxabi.h>
#include <harness/hal/file_system.h>
#include <harness/utils/expected.h>

#include <outpost/hal/file_system.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string_view>

namespace harness
{
namespace hal
{

// ---------------------------------------------------------------------------
/**
 * \class TestHelper
 */
class TestHelper
{
public:
    explicit TestHelper() = default;

    virtual ~TestHelper() = default;

    virtual std::string
    toAbsPath(const std::string& path) const = 0;

    virtual void
    clearWorkspace() = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class FileSystemTestSuiteRaw
 */
class FileSystemTestSuiteRaw : public ::testing::Test
{
public:
    static constexpr bool readOnly = true;

    using Permission = outpost::hal::filesystem::Permission;

    static constexpr const char* anyFileName = "test.txt";
    static constexpr const char* otherFileName = "other.txt";
    static constexpr const std::string_view anyContent{"test123"};

    static constexpr std::array<Permission, 7U> allPermissions = {Permission::R(),
                                                                  Permission::W(),
                                                                  Permission::X(),
                                                                  Permission::W() | Permission::R(),
                                                                  Permission::W() | Permission::X(),
                                                                  Permission::R() | Permission::X(),
                                                                  Permission::RWX()};

    FileSystemTestSuiteRaw(outpost::hal::FileSystem& fs, TestHelper& helper);

    void
    SetUp() override;

    void
    TearDown() override;

    void
    mount(bool mountReadOnly = false);

    void
    unmount();

    outpost::hal::FileSystem::Result
    createFile(const std::string& path);

    outpost::hal::FileSystem::Result
    createFile(const std::string& path, const Permission& permission);

    void
    createFile(const std::string& path, const std::string_view& content);

    void
    createFile(const std::string& path,
               const std::string_view& content,
               const Permission& permission);

    outpost::hal::FileSystem::Result
    mkDir(const std::string& path, const Permission& permission = Permission::RWX());

    void
    checkFileExists(const std::string& path);

    void
    checkFileContent(const std::string& path, const std::string_view& content);

    void
    checkPermission(const std::string& path, const Permission& permission);

    std::string
    toAbsPath(const std::string& path) const;

    std::string
    createTooLongPath() const;

    outpost::hal::FileSystem& mFileSystem;
    TestHelper& mHelper;
};

// ---------------------------------------------------------------------------
/**
 * \class FileSystemTestSuite
 *
 * \tparam FileSystemImpl
 *      The implementation of the file system (unit under test). Must provide
 *      a default constructor and a type `FileSystemImpl::TestHelper`, which
 *      implements the interface `TestHelper`.
 */
template <typename FileSystemImpl>
class FileSystemTestSuite : public FileSystemTestSuiteRaw
{
public:
    FileSystemTestSuite() : FileSystemTestSuiteRaw(mInstance, mHelper)
    {
    }

private:
    FileSystemImpl mInstance;
    typename FileSystemImpl::TestHelper mHelper;
};

// ---------------------------------------------------------------------------
class NamePrinter
{
public:
    template <class ParamType>
    static std::string
    GetName(int /* num */)
    {
        return demangled(typeid(ParamType).name());
    }

private:
    static std::string
    demangled(char const* name)
    {
        std::unique_ptr<char, void (*)(void*)> demangledName{
                abi::__cxa_demangle(name, 0, 0, nullptr), std::free};
        return std::string{demangledName.get()};
    }
};

// ---------------------------------------------------------------------------
TYPED_TEST_SUITE_P(FileSystemTestSuite);

// ---------------------------------------------------------------------------
#include "file_system_test_suite_create_file.h"
#include "file_system_test_suite_read.h"
#include "file_system_test_suite_write.h"

// ---------------------------------------------------------------------------
// https://stackoverflow.com/a/35212530
#define EXPAND(x) x
#define PAREN(...) (__VA_ARGS__)
#define EXPAND_F(m, ...) EXPAND(m PAREN(__VA_ARGS__))

EXPAND_F(REGISTER_TYPED_TEST_SUITE_P,
         FileSystemTestSuite,
         CREATE_FILE_TESTS,
         READ_TESTS,
         WRITE_TESTS);

}  // namespace hal
}  // namespace harness

#endif  // HARNESS_HAL_FILE_SYSTEM_TEST_SUITE_H
