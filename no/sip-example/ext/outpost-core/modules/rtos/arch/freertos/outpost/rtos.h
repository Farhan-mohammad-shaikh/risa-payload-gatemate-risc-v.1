/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2014, Norbert Toth
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_RTOS_H
#define OUTPOST_RTOS_H

/**
 * \defgroup rtos   RTOS Wrappers
 * \brief    Real Time Operating System
 *
 * This library provide an operating system abstraction layer.
 */

#include "rtos/clock.h"
#include "rtos/mutex.h"
#include "rtos/periodic_task_manager.h"
#include "rtos/queue.h"
#include "rtos/semaphore.h"
#include "rtos/thread.h"
#include "rtos/timer.h"

#include <outpost/base/callable.h>
#include <outpost/rtos/failure_handler.h>
#include <outpost/rtos/mutex_guard.h>

#endif
