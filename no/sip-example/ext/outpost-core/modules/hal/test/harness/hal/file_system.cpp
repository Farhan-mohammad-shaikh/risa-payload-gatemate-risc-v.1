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

#include "harness/hal/file_system.h"

#include <outpost/base/testing_assert.h>

// ---------------------------------------------------------------------------
void
outpost::hal::PrintTo(const outpost::hal::FileSystem::Result& result, std::ostream* os)
{
    switch (result)
    {
        case (outpost::hal::FileSystem::Result::success): {
            *os << "success";
            break;
        }
        case (outpost::hal::FileSystem::Result::endOfData): {
            *os << "endOfData";
            break;
        }
        case (outpost::hal::FileSystem::Result::notFound): {
            *os << "notFound";
            break;
        }
        case (outpost::hal::FileSystem::Result::accessDenied): {
            *os << "accessDenied";
            break;
        }
        case (outpost::hal::FileSystem::Result::noSpace): {
            *os << "noSpace";
            break;
        }
        case (outpost::hal::FileSystem::Result::readOnly): {
            *os << "readOnly";
            break;
        }
        case (outpost::hal::FileSystem::Result::writeOnly): {
            *os << "writeOnly";
            break;
        }
        case (outpost::hal::FileSystem::Result::alreadyExists): {
            *os << "alreadyExists";
            break;
        }
        case (outpost::hal::FileSystem::Result::notEmpty): {
            *os << "notEmpty";
            break;
        }
        case (outpost::hal::FileSystem::Result::notAFile): {
            *os << "notAFile";
            break;
        }
        case (outpost::hal::FileSystem::Result::notADirectory): {
            *os << "notADirectory";
            break;
        }
        case (outpost::hal::FileSystem::Result::invalidInput): {
            *os << "invalidInput";
            break;
        }
        case (outpost::hal::FileSystem::Result::resourceExhausted): {
            *os << "resourceExhausted";
            break;
        }
        case (outpost::hal::FileSystem::Result::invalidState): {
            *os << "invalidState";
            break;
        }
        case (outpost::hal::FileSystem::Result::IOError): {
            *os << "IOError";
            break;
        }
        case (outpost::hal::FileSystem::Result::notImplemented): {
            *os << "notImplemented";
            break;
        }
        case (outpost::hal::FileSystem::Result::fileInUse): {
            *os << "fileInUse";
            break;
        }
        case (outpost::hal::FileSystem::Result::other): {
            *os << "other";
            break;
        }
        default: OUTPOST_ASSERT(false, "Invalid FilesystemResult"); break;
    }
}

void
outpost::hal::filesystem::PrintTo(const outpost::hal::filesystem::Permission& permission,
                                  std::ostream* os)
{
    if (permission.isReadable())
    {
        *os << "R";
    }

    if (permission.isWritable())
    {
        *os << "W";
    }

    if (permission.isExecutable())
    {
        *os << "X";
    }
}
