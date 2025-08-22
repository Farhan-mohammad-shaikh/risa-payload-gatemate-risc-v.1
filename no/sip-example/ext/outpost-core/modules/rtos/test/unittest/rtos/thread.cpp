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

#include "outpost/rtos/thread.h"

#include "unittest/rtos/thread_mock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
Thread::Thread(uint8_t priority,
               size_t stack,
               const char* name,
               FloatingPointSupport floatingPointSupport,
               CpuMask affinity) :
    mImplementation(nullptr)
{
    auto* thread = new unittest::rtos::ThreadImplementation(
            this, priority, stack, name, floatingPointSupport, affinity);
    mImplementation = thread;
}

Thread::~Thread()
{
    auto* thread = reinterpret_cast<unittest::rtos::ThreadImplementation*>(mImplementation);
    delete thread;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
Thread::Identifier
Thread::getIdentifier() const
{
    auto* thread = reinterpret_cast<unittest::rtos::ThreadImplementation*>(mImplementation);
    return thread->getIdentifier();
}

Thread::Identifier
Thread::getCurrentThreadIdentifier()
{
    auto* thread = unittest::rtos::ThreadImplementation::getCurrentThread();
    return thread->getCurrentThreadIdentifier();
}

void
Thread::start()
{
    auto* thread = reinterpret_cast<unittest::rtos::ThreadImplementation*>(mImplementation);
    return thread->start();
}

void
Thread::setPriority(uint8_t priority)
{
    auto* thread = reinterpret_cast<unittest::rtos::ThreadImplementation*>(mImplementation);
    return thread->setPriority(priority);
}

uint8_t
Thread::getPriority() const
{
    auto* thread = reinterpret_cast<unittest::rtos::ThreadImplementation*>(mImplementation);
    return thread->getPriority();
}

void
Thread::yield()
{
    auto* thread = unittest::rtos::ThreadImplementation::getCurrentThread();
    return thread->yield();
}

void
Thread::sleep(::outpost::time::Duration timeout)
{
    auto* thread = unittest::rtos::ThreadImplementation::getCurrentThread();
    return thread->sleep(timeout);
}
