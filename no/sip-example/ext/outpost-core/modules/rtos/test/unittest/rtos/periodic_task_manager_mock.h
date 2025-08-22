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

#ifndef UNITTEST_RTOS_PERIODIC_TASK_MANAGER_MOCK_H
#define UNITTEST_RTOS_PERIODIC_TASK_MANAGER_MOCK_H

#include <outpost/posix/periodic_task_manager.h>
#include <outpost/rtos/periodic_task_manager.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class PeriodicTaskManagerImplementation;

// ---------------------------------------------------------------------------
/**
 * \class PeriodicTaskManagerInterface
 */
class PeriodicTaskManagerInterface
{
public:
    explicit PeriodicTaskManagerInterface() = default;

    PeriodicTaskManagerInterface(const PeriodicTaskManagerInterface&) = delete;

    PeriodicTaskManagerInterface&
    operator=(const PeriodicTaskManagerInterface&) = delete;

    virtual ~PeriodicTaskManagerInterface() = default;

    virtual outpost::rtos::PeriodicTaskManager::Status::Type
    nextPeriod(outpost::time::Duration period) = 0;

    virtual outpost::rtos::PeriodicTaskManager::Status::Type
    status() = 0;

    virtual void
    cancel() = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class PeriodicTaskManagerMock
 */
class PeriodicTaskManagerMock : public PeriodicTaskManagerInterface,
                                public Mock<PeriodicTaskManagerImplementation>,
                                public outpost::ListElement
{
public:
    explicit PeriodicTaskManagerMock(MockMode mode = defaultMode) :
        Mock<PeriodicTaskManagerImplementation>(mode)
    {
    }

    friend void
    unittest::rtos::registerMock(PeriodicTaskManagerMock&);
    friend class PeriodicTaskManagerImplementation;

    ~PeriodicTaskManagerMock();

    MOCK_METHOD(outpost::rtos::PeriodicTaskManager::Status::Type,
                nextPeriod,
                (outpost::time::Duration),
                (override));
    MOCK_METHOD(outpost::rtos::PeriodicTaskManager::Status::Type, status, (), (override));
    MOCK_METHOD(void, cancel, (), (override));

private:
    static outpost::List<PeriodicTaskManagerMock> registeredMocks;
};

// ---------------------------------------------------------------------------
/**
 * \class PeriodicTaskManagerImplementation
 */
class PeriodicTaskManagerImplementation
    : public Mockable<PeriodicTaskManagerMock, outpost::posix::PeriodicTaskManager>
{
public:
    explicit PeriodicTaskManagerImplementation(outpost::rtos::PeriodicTaskManager* manager);

    PeriodicTaskManagerImplementation(const PeriodicTaskManagerImplementation&) = delete;

    PeriodicTaskManagerImplementation&
    operator=(const PeriodicTaskManagerImplementation&) = delete;

    ~PeriodicTaskManagerImplementation() = default;

    outpost::rtos::PeriodicTaskManager::Status::Type
    nextPeriod(outpost::time::Duration period);

    outpost::rtos::PeriodicTaskManager::Status::Type
    status();

    void
    cancel();

private:
    outpost::posix::PeriodicTaskManager mPeriodicTaskManager;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_PERIODIC_TASK_MANAGER_MOCK_H
