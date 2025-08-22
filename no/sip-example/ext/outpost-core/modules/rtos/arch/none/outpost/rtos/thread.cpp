/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2023, Hannah Kirstein
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "thread.h"

#include <outpost/rtos/failure_handler.h>

#include <stdio.h>

// ----------------------------------------------------------------------------
outpost::rtos::Thread::Thread(uint8_t priority,
                              size_t stack,
                              const char* name,
                              FloatingPointSupport /*floatingPointSupport*/,
                              CpuMask /*affinity*/)
{
    (void) priority;
    (void) stack;
    (void) name;
}

outpost::rtos::Thread::~Thread()
{
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::start()
{
}

// ----------------------------------------------------------------------------
outpost::rtos::Thread::Identifier
outpost::rtos::Thread::getIdentifier() const
{
    return 0;
}

outpost::rtos::Thread::Identifier
outpost::rtos::Thread::getCurrentThreadIdentifier()
{
    return 0;
}

// ----------------------------------------------------------------------------
void
outpost::rtos::Thread::setPriority(uint8_t priority)
{
    (void) priority;
}

uint8_t
outpost::rtos::Thread::getPriority() const
{
    return 0;
}

void __attribute__((weak)) outpost::rtos::Thread::sleep(::outpost::time::Duration duration)
{
    (void) duration;
}
