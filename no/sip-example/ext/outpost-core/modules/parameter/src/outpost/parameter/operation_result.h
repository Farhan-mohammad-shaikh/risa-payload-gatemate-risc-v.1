/*
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2023, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_PARAMETER_OPERATIONRESULT_H_
#define OUTPOST_PARAMETER_OPERATIONRESULT_H_

namespace outpost
{
namespace parameter
{
enum class OperationResult
{
    success,                  // everything is fine
    incorrectType,            // requested type that was not correct
    concurrentWrite,          // write failed due to another write currently in progress
    notInitialized,           // Function called before object initialized
    alreadyInitialized,       // tried to initialized more than once
    invalidParameter,         // Parameters are not valid
    tooManyConcurrentWrites,  // read failed because there were at least maxReadTries writes during
                              // execution
    noSuchID,                 // no Parameter exists for the requested ID
    duplicatedID,             // two or more Parameter in the list have identical IDs
    dublicatedID = duplicatedID,  // backwards-compatibility with older projects
    uninitializedParameter,       // at least one Parameter was not initialized
    tooManyElements               // the list contains more than maxElements Parameter
};
}
}  // namespace outpost

#endif
