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

#include "outpost/posix/periodic_task_manager.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
PeriodicTaskManager::PeriodicTaskManager() : mImplementation(nullptr)
{
    auto* ptm = new outpost::posix::PeriodicTaskManager();
    mImplementation = ptm;
}

PeriodicTaskManager::~PeriodicTaskManager()
{
    auto* ptm = reinterpret_cast<outpost::posix::PeriodicTaskManager*>(mImplementation);
    delete ptm;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
PeriodicTaskManager::Status::Type
PeriodicTaskManager::nextPeriod(time::Duration period)
{
    auto* ptm = reinterpret_cast<outpost::posix::PeriodicTaskManager*>(mImplementation);
    return static_cast<PeriodicTaskManager::Status::Type>(ptm->nextPeriod(period));
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::status()
{
    auto* ptm = reinterpret_cast<outpost::posix::PeriodicTaskManager*>(mImplementation);
    return static_cast<PeriodicTaskManager::Status::Type>(ptm->status());
}

void
PeriodicTaskManager::cancel()
{
    auto* ptm = reinterpret_cast<outpost::posix::PeriodicTaskManager*>(mImplementation);
    return ptm->cancel();
}
