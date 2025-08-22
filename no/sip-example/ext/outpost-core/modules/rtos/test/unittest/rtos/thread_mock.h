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

#ifndef UNITTEST_RTOS_THREAD_MOCK_H
#define UNITTEST_RTOS_THREAD_MOCK_H

#include <outpost/posix/thread.h>
#include <outpost/rtos/thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class ThreadImplementation;

// ---------------------------------------------------------------------------
/**
 * \class PosixThreadWrapper
 */
class PosixThreadWrapper : public outpost::posix::Thread
{
public:
    explicit PosixThreadWrapper(outpost::rtos::Thread* thread,
                                uint8_t priority,
                                size_t stack,
                                const char* name,
                                outpost::rtos::Thread::FloatingPointSupport floatingPointSupport,
                                outpost::rtos::CpuMask affinity);

    PosixThreadWrapper(const PosixThreadWrapper&) = delete;

    PosixThreadWrapper&
    operator=(const PosixThreadWrapper&) = delete;

    ~PosixThreadWrapper() = default;

private:
    void
    run() override;

    outpost::rtos::Thread* mThread;
};

// ---------------------------------------------------------------------------
/**
 * \class DummyThread
 */
class DummyThread : public outpost::rtos::Thread
{
public:
    explicit DummyThread() : outpost::rtos::Thread(0U)
    {
    }

    DummyThread(const DummyThread&) = delete;

    DummyThread&
    operator=(const DummyThread&) = delete;

    ~DummyThread() = default;

    void
    run() override
    {
    }

private:
};

// ---------------------------------------------------------------------------
/**
 * \class ExecuteAs
 *
 * Set the current thread via RAII.
 *
 * \code
 *
 * unittest::rtos::ThreadMock mock1;
 * unittest::rtos::ThreadMock mock2;
 *
 * MyThread thread1;
 * MyThread thread2;
 *
 * unittest::rtos::injectMock(thread1, mock1);
 * unittest::rtos::injectMock(thread2, mock2);
 *
 * ExecuteAs main{thread2};
 *
 * {
 *      ExecuteAs current{thread1};
 *      doSomething(); // static method calls will be forwarded to mock1
 * }
 *
 * doSomething(); // static method calls will be forwarded to mock2
 *
 * \code
 */
class ExecuteAs
{
public:
    explicit ExecuteAs(outpost::rtos::Thread& thread);

    ExecuteAs(const ExecuteAs&) = delete;

    ExecuteAs&
    operator=(const ExecuteAs&) = delete;

    ~ExecuteAs();

private:
    ThreadImplementation* mOld;
};

// ---------------------------------------------------------------------------
/**
 * \class ThreadInterface
 */
class ThreadInterface
{
public:
    using Identifier = outpost::rtos::Thread::Identifier;

    explicit ThreadInterface() = default;

    ThreadInterface(const ThreadInterface&) = delete;

    ThreadInterface&
    operator=(const ThreadInterface&) = delete;

    virtual ~ThreadInterface() = default;

    virtual void
    start() = 0;

    virtual Identifier
    getIdentifier() const = 0;

    virtual Identifier
    getCurrentThreadIdentifier() = 0;

    virtual void
    setPriority(uint8_t priority) = 0;

    virtual uint8_t
    getPriority() const = 0;

    virtual void
    yield() = 0;

    virtual void
    sleep(::outpost::time::Duration timeout) = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class ThreadMock
 */
class ThreadMock : public ThreadInterface, public Mock<ThreadImplementation>
{
public:
    explicit ThreadMock(MockMode mode = defaultMode) : Mock<ThreadImplementation>(mode)
    {
    }

    MOCK_METHOD(void, start, (), (override));
    MOCK_METHOD(Identifier, getIdentifier, (), (const, override));
    MOCK_METHOD(Identifier, getCurrentThreadIdentifier, (), (override));
    MOCK_METHOD(void, setPriority, (uint8_t), (override));
    MOCK_METHOD(uint8_t, getPriority, (), (const, override));
    MOCK_METHOD(void, yield, (), (override));
    MOCK_METHOD(void, sleep, (::outpost::time::Duration), (override));
};

// ---------------------------------------------------------------------------
/**
 * \class ThreadImplementation
 */
class ThreadImplementation : public Mockable<ThreadMock, outpost::posix::Thread>
{
public:
    friend class ExecuteAs;

    using Identifier = outpost::rtos::Thread::Identifier;

    static Identifier nextIdentifier;

    /**
     * Returns the current thread.
     *
     * The user can set the current thread by creating a instance of
     * \c ExecuteAs \c. The thread passed to the constructor will be
     * "active". If no thread is explicit set by the user, the first thread
     * of \c allInstances will be used as the current thread. To explicit
     * manager the current thread is necessary since most of part of the thread
     * API is declared as static. Therefore the mapping to a thread must be
     * explicit established.
     *
     * \return the current thread if a thread is active, otherwise nullptr.
     */
    static ThreadImplementation*
    getCurrentThread();

    explicit ThreadImplementation(outpost::rtos::Thread* thread,
                                  uint8_t priority,
                                  size_t stack,
                                  const char* name,
                                  outpost::rtos::Thread::FloatingPointSupport floatingPointSupport,
                                  outpost::rtos::CpuMask affinity);

    ThreadImplementation(const ThreadImplementation&) = delete;

    ThreadImplementation&
    operator=(const ThreadImplementation&) = delete;

    ~ThreadImplementation();

    void
    start();

    Identifier
    getIdentifier() const;

    Identifier
    getCurrentThreadIdentifier();

    void
    setPriority(uint8_t priority);

    uint8_t
    getPriority() const;

    void
    yield();

    void
    sleep(::outpost::time::Duration timeout);

    /**
     * Execute the threads run()-method
     */
    void
    executeThread();

    /**
     * Get the name passed to the thread instance via the constructor.
     *
     * \return the name of the thread
     */
    const char*
    getName() const;

private:
    static ThreadImplementation* currentThread;

    PosixThreadWrapper mPosixThread;

    outpost::rtos::Thread* const mThread;
    const char* mName;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_THREAD_MOCK_H
