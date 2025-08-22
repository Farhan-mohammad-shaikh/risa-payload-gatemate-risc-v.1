/*
 * Copyright (c) 2014-2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "periodic_task_manager.h"

using namespace outpost::rtos;

PeriodicTaskManager::PeriodicTaskManager()
{
}

PeriodicTaskManager::~PeriodicTaskManager()
{
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::nextPeriod(time::Duration period)
{
    (void) period;
    return Status::idle;
}

PeriodicTaskManager::Status::Type
PeriodicTaskManager::status()
{
    return Status::idle;
}

void
PeriodicTaskManager::cancel()
{
}
