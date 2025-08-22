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

#include "unittest/rtos/thread_mock.h"

using namespace unittest::rtos;

// ---------------------------------------------------------------------------
ThreadImplementation::Identifier ThreadImplementation::nextIdentifier = 0;
ThreadImplementation* ThreadImplementation::currentThread = nullptr;

// ---------------------------------------------------------------------------
void
unittest::rtos::runWrapper(outpost::rtos::Thread& thread)
{
    return thread.run();
}

// ---------------------------------------------------------------------------
PosixThreadWrapper::PosixThreadWrapper(
        outpost::rtos::Thread* thread,
        uint8_t priority,
        size_t stack,
        const char* name,
        outpost::rtos::Thread::FloatingPointSupport floatingPointSupport,
        outpost::rtos::CpuMask affinity) :
    outpost::posix::Thread(
            priority,
            stack,
            name,
            static_cast<outpost::posix::Thread::FloatingPointSupport>(floatingPointSupport),
            affinity),
    mThread(thread)
{
}

void
PosixThreadWrapper::run()
{
    return unittest::rtos::runWrapper(*mThread);
}

// ---------------------------------------------------------------------------
ExecuteAs::ExecuteAs(outpost::rtos::Thread& thread) : mOld(nullptr)
{
    auto iter = ThreadImplementation::allInstances.begin();
    ThreadImplementation* impl = nullptr;
    while (iter != ThreadImplementation::allInstances.end())
    {
        if (iter->getAddress() == reinterpret_cast<intptr_t>(&thread))
        {
            impl = &*iter;
        }
        ++iter;
    }

    mOld = ThreadImplementation::currentThread;
    ThreadImplementation::currentThread = impl;
}

ExecuteAs::~ExecuteAs()
{
    ThreadImplementation::currentThread = mOld;
}

// ---------------------------------------------------------------------------
ThreadImplementation*
ThreadImplementation::getCurrentThread()
{
    if (currentThread == nullptr)
    {
        if (allInstances.size() > 0)
        {
            return allInstances.first();
        }
    }
    return currentThread;
}

// ---------------------------------------------------------------------------
ThreadImplementation::ThreadImplementation(
        outpost::rtos::Thread* thread,
        uint8_t priority,
        size_t stack,
        const char* name,
        outpost::rtos::Thread::FloatingPointSupport floatingPointSupport,
        outpost::rtos::CpuMask affinity) :
    Mockable<ThreadMock, outpost::posix::Thread>(
            this, reinterpret_cast<intptr_t>(thread), &mPosixThread),
    mPosixThread(thread, priority, stack, name, floatingPointSupport, affinity),
    mThread(thread),
    mName(name)
{
}

ThreadImplementation::~ThreadImplementation()
{
}

// ---------------------------------------------------------------------------
void
ThreadImplementation::start()
{
    std::function<void()> mockFunction = std::bind(&ThreadMock::start, mMock);
    std::function<void()> realFunction = std::bind(&PosixThreadWrapper::start, &mPosixThread);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

ThreadImplementation::Identifier
ThreadImplementation::getIdentifier() const
{
    std::function<Identifier()> mockFunction = std::bind(&ThreadMock::getIdentifier, mMock);
    std::function<Identifier()> realFunction =
            std::bind(&PosixThreadWrapper::getIdentifier, &mPosixThread);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

ThreadImplementation::Identifier
ThreadImplementation::getCurrentThreadIdentifier()
{
    std::function<Identifier()> mockFunction =
            std::bind(&ThreadMock::getCurrentThreadIdentifier, mMock);
    std::function<Identifier()> realFunction =
            std::bind(&PosixThreadWrapper::getCurrentThreadIdentifier);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
ThreadImplementation::setPriority(uint8_t priority)
{
    std::function<void()> mockFunction = std::bind(&ThreadMock::setPriority, mMock, priority);
    std::function<void()> realFunction =
            std::bind(&PosixThreadWrapper::setPriority, &mPosixThread, priority);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

uint8_t
ThreadImplementation::getPriority() const
{
    std::function<uint8_t()> mockFunction = std::bind(&ThreadMock::getPriority, mMock);
    std::function<uint8_t()> realFunction =
            std::bind(&PosixThreadWrapper::getPriority, &mPosixThread);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
ThreadImplementation::yield()
{
    std::function<void()> mockFunction = std::bind(&ThreadMock::yield, mMock);
    std::function<void()> realFunction = std::bind(&PosixThreadWrapper::yield);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
ThreadImplementation::sleep(::outpost::time::Duration timeout)
{
    std::function<void()> mockFunction = std::bind(&ThreadMock::sleep, mMock, timeout);
    std::function<void()> realFunction = std::bind(&PosixThreadWrapper::sleep, timeout);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

// ---------------------------------------------------------------------------
void
ThreadImplementation::executeThread()
{
    unittest::rtos::ExecuteAs executeAs(*mThread);
    runWrapper(*mThread);
}

const char*
ThreadImplementation::getName() const
{
    return mName;
}
