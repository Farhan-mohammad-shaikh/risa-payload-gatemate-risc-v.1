/*
 * Copyright (c) 2013-2017, 2019, Fabian Greif
 * Copyright (c) 2024, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_PERIODIC_TASK_MANAGER_H
#define OUTPOST_RTOS_PERIODIC_TASK_MANAGER_H

#include "outpost/posix/periodic_task_manager.h"

namespace outpost
{
namespace rtos
{

using PeriodicTaskManager = outpost::posix::PeriodicTaskManager;

}  // namespace rtos
}  // namespace outpost

#endif
