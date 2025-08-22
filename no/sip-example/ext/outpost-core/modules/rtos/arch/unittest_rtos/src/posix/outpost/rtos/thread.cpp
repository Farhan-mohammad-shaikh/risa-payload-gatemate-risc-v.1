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

#include "outpost/posix/thread.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
void
unittest::rtos::runWrapper(outpost::rtos::Thread& thread)
{
    return thread.run();
}

// ---------------------------------------------------------------------------
namespace outpost
{
namespace rtos
{

/**
 * \class ThreadWrapper
 */
class ThreadWrapper : public outpost::posix::Thread
{
public:
    explicit ThreadWrapper(outpost::rtos::Thread& thread,
                           uint8_t priority,
                           size_t stack,
                           const char* name,
                           FloatingPointSupport floatingPointSupport,
                           rtos::CpuMask affinity) :
        outpost::posix::Thread(priority, stack, name, floatingPointSupport, affinity),
        mThread(thread)
    {
    }

    ThreadWrapper(const ThreadWrapper&) = delete;

    ThreadWrapper&
    operator=(const ThreadWrapper&) = delete;

    ~ThreadWrapper() = default;

private:
    void
    run() override
    {
        unittest::rtos::runWrapper(mThread);
    }

    outpost::rtos::Thread& mThread;
};

}  // namespace rtos
}  // namespace outpost

// ---------------------------------------------------------------------------
Thread::Thread(uint8_t priority,
               size_t stack,
               const char* name,
               FloatingPointSupport floatingPointSupport,
               CpuMask affinity) :
    mImplementation(nullptr)
{
    const auto fp = static_cast<outpost::posix::Thread::FloatingPointSupport>(floatingPointSupport);
    auto* thread = new ThreadWrapper(*this, priority, stack, name, fp, affinity);
    mImplementation = thread;
}

Thread::~Thread()
{
    auto* thread = reinterpret_cast<ThreadWrapper*>(mImplementation);
    delete thread;
    mImplementation = nullptr;
}

Thread::Identifier
Thread::getIdentifier() const
{
    auto* thread = reinterpret_cast<ThreadWrapper*>(mImplementation);
    return thread->getIdentifier();
}

Thread::Identifier
Thread::getCurrentThreadIdentifier()
{
    return outpost::posix::Thread::getCurrentThreadIdentifier();
}

void
Thread::start()
{
    auto* thread = reinterpret_cast<ThreadWrapper*>(mImplementation);
    return thread->start();
}

void
Thread::setPriority(uint8_t priority)
{
    auto* thread = reinterpret_cast<ThreadWrapper*>(mImplementation);
    return thread->setPriority(priority);
}

uint8_t
Thread::getPriority() const
{
    auto* thread = reinterpret_cast<ThreadWrapper*>(mImplementation);
    return thread->getPriority();
}

void
Thread::yield()
{
    return outpost::posix::Thread::yield();
}

void
Thread::sleep(::outpost::time::Duration timeout)
{
    return outpost::posix::Thread::sleep(timeout);
}
