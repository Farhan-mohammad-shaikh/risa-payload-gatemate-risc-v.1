/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "unittest/rtos/periodic_task_manager_mock.h"

using namespace unittest::rtos;
using namespace outpost::rtos;

// ---------------------------------------------------------------------------
outpost::List<PeriodicTaskManagerMock> PeriodicTaskManagerMock::registeredMocks;

// ---------------------------------------------------------------------------
static PeriodicTaskManager::Status::Type
wrapper(std::function<outpost::posix::PeriodicTaskManager::Status::Type()>& function)
{
    return static_cast<PeriodicTaskManager::Status::Type>(function());
}

// ---------------------------------------------------------------------------
PeriodicTaskManagerMock::~PeriodicTaskManagerMock()
{
    auto iter = registeredMocks.begin();
    if (iter != registeredMocks.end())
    {
        // was the mock used?
        assert(&*iter != this);
        // cppcheck-suppress unreadVariable
        ++iter;
    }
}

// ---------------------------------------------------------------------------
PeriodicTaskManagerImplementation::PeriodicTaskManagerImplementation(PeriodicTaskManager* manager) :
    Mockable<PeriodicTaskManagerMock, outpost::posix::PeriodicTaskManager>(
            this, reinterpret_cast<intptr_t>(manager), &mPeriodicTaskManager)
{
    if (PeriodicTaskManagerMock::registeredMocks.size() > 0U)
    {
        auto* mock = PeriodicTaskManagerMock::registeredMocks.first();
        PeriodicTaskManagerMock::registeredMocks.removeFirst();
        setMock(*mock);
    }
}

// ---------------------------------------------------------------------------
PeriodicTaskManager::Status::Type
PeriodicTaskManagerImplementation::nextPeriod(outpost::time::Duration period)
{
    std::function<PeriodicTaskManager::Status::Type()> mockFunction =
            std::bind(&PeriodicTaskManagerMock::nextPeriod, mMock, period);
    std::function<outpost::posix::PeriodicTaskManager::Status::Type()> realFunction = std::bind(
            &outpost::posix::PeriodicTaskManager::nextPeriod, &mPeriodicTaskManager, period);
    std::function<PeriodicTaskManager::Status::Type()> realFunctionWrapper =
            std::bind(&wrapper, realFunction);
    return handleMethodCall(mMock, mockFunction, realFunctionWrapper);
}

PeriodicTaskManager::Status::Type
PeriodicTaskManagerImplementation::status()
{
    std::function<PeriodicTaskManager::Status::Type()> mockFunction =
            std::bind(&PeriodicTaskManagerMock::status, mMock);
    std::function<outpost::posix::PeriodicTaskManager::Status::Type()> realFunction =
            std::bind(&outpost::posix::PeriodicTaskManager::status, &mPeriodicTaskManager);
    std::function<PeriodicTaskManager::Status::Type()> realFunctionWrapper =
            std::bind(&wrapper, realFunction);
    return handleMethodCall(mMock, mockFunction, realFunctionWrapper);
}

void
PeriodicTaskManagerImplementation::cancel()
{
    std::function<void()> mockFunction = std::bind(&PeriodicTaskManagerMock::cancel, mMock);
    std::function<void()> realFunction =
            std::bind(&outpost::posix::PeriodicTaskManager::cancel, &mPeriodicTaskManager);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
