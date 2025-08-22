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

#include "unittest/rtos/mock.h"

#include "unittest/rtos/periodic_task_manager_mock.h"

using namespace unittest::rtos;

// ---------------------------------------------------------------------------
MockRaw::MockRaw(MockMode mode) : mMode(mode)
{
}

MockMode
MockRaw::getMode() const
{
    return mMode;
}

// ---------------------------------------------------------------------------
void
unittest::rtos::registerMock(PeriodicTaskManagerMock& mock)
{
    auto iter = PeriodicTaskManagerMock::registeredMocks.begin();
    while (iter != PeriodicTaskManagerMock::registeredMocks.end())
    {
        // should not be registered twice
        assert(&*iter != &mock);
        ++iter;
    }

    PeriodicTaskManagerMock::registeredMocks.append(&mock);
}
