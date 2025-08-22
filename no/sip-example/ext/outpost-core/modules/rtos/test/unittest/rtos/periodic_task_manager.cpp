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

#include "outpost/rtos/periodic_task_manager.h"

#include "unittest/rtos/periodic_task_manager_mock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
PeriodicTaskManager::PeriodicTaskManager() : mImplementation(nullptr)
{
    auto* manager = new unittest::rtos::PeriodicTaskManagerImplementation(this);
    mImplementation = manager;
}

PeriodicTaskManager::~PeriodicTaskManager()
{
    auto* manager =
            reinterpret_cast<unittest::rtos::PeriodicTaskManagerImplementation*>(mImplementation);
    delete manager;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
PeriodicTaskManager::Status::Type
PeriodicTaskManager::nextPeriod(time::Duration period)
{
    auto* manager =
            reinterpret_cast<unittest::rtos::PeriodicTaskManagerImplementation*>(mImplementation);
    return manager->nextPeriod(period);
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::status()
{
    auto* manager =
            reinterpret_cast<unittest::rtos::PeriodicTaskManagerImplementation*>(mImplementation);
    return manager->status();
}

void
PeriodicTaskManager::cancel()
{
    auto* manager =
            reinterpret_cast<unittest::rtos::PeriodicTaskManagerImplementation*>(mImplementation);
    return manager->cancel();
}
