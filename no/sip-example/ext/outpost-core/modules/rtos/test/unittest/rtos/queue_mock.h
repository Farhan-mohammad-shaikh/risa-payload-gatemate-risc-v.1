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

#ifndef UNITTEST_RTOS_QUEUE_MOCK_H
#define UNITTEST_RTOS_QUEUE_MOCK_H

#include <outpost/posix/queue_raw.h>
#include <outpost/rtos/queue.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "unittest/rtos/mock.h"

namespace unittest
{
namespace rtos
{

class QueueImplementation;

// ---------------------------------------------------------------------------
/**
 * \class QueueInterface
 */
template <typename T>
class QueueInterface
{
public:
    explicit QueueInterface() = default;

    QueueInterface(const QueueInterface&) = delete;

    QueueInterface&
    operator=(const QueueInterface&) = delete;

    virtual ~QueueInterface() = default;

    virtual bool
    send(const T& data) = 0;

    virtual bool
    sendFromISR(const T& data, bool& hasWokenTask) = 0;

    virtual bool
    receive(T& data, outpost::time::Duration timeout) = 0;

    virtual bool
    receiveFromISR(T& data, bool& hasWokenTask) = 0;

    virtual void
    clear() = 0;

    virtual size_t
    getNumberOfPendingMessages() = 0;

private:
};

// ---------------------------------------------------------------------------
/**
 * \class QueueMockRaw
 */
class QueueMockRaw : public Mock<QueueImplementation>
{
public:
    explicit QueueMockRaw(MockMode mode, const std::type_info& info);

    QueueMockRaw(const QueueMockRaw&) = delete;

    QueueMockRaw&
    operator=(const QueueMockRaw&) = delete;

    virtual ~QueueMockRaw() = default;

    virtual bool
    sendRaw(const void* data) = 0;

    virtual bool
    sendFromISRRaw(const void* data, bool& hasWokenTask) = 0;

    virtual bool
    receiveRaw(void* data, outpost::time::Duration timeout) = 0;

    virtual bool
    receiveFromISRRaw(void* data, bool& hasWokenTask) = 0;

    virtual void
    clearRaw() = 0;

    virtual size_t
    getNumberOfPendingMessagesRaw() = 0;

    const std::type_info&
    getTypeInfo() const;

private:
    const std::type_info& mInfo;
};

/**
 * \class QueueMock
 */
template <typename T>
class QueueMock : public QueueInterface<T>, public QueueMockRaw
{
public:
    explicit QueueMock(MockMode mode = defaultMode) : QueueMockRaw(mode, typeid(T))
    {
    }

    MOCK_METHOD(bool, send, (const T&), (override));
    MOCK_METHOD(bool, sendFromISR, (const T&, bool&), (override));
    MOCK_METHOD(bool, receive, (T&, outpost::time::Duration), (override));
    MOCK_METHOD(bool, receiveFromISR, (T&, bool&), (override));
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(size_t, getNumberOfPendingMessages, (), (override));

    bool
    sendRaw(const void* data) override
    {
        return send(*reinterpret_cast<const T*>(data));
    }

    bool
    sendFromISRRaw(const void* data, bool& hasWokenTask) override
    {
        return sendFromISR(*reinterpret_cast<const T*>(data), hasWokenTask);
    }

    bool
    receiveRaw(void* data, outpost::time::Duration timeout) override
    {
        return receive(*reinterpret_cast<T*>(data), timeout);
    }

    bool
    receiveFromISRRaw(void* data, bool& hasWokenTask) override
    {
        return receiveFromISR(*reinterpret_cast<T*>(data), hasWokenTask);
    }

    void
    clearRaw() override
    {
        return clear();
    }

    size_t
    getNumberOfPendingMessagesRaw() override
    {
        return getNumberOfPendingMessages();
    }
};

// ---------------------------------------------------------------------------
/**
 * \class QueueImplementation
 */
class QueueImplementation : public Mockable<QueueMockRaw, outpost::posix::QueueRaw>
{
public:
    explicit QueueImplementation(outpost::rtos::QueueRaw* queue,
                                 size_t numberOfItems,
                                 size_t itemSize,
                                 const std::type_info& info);

    QueueImplementation(const QueueImplementation&) = delete;

    QueueImplementation&
    operator=(const QueueImplementation&) = delete;

    ~QueueImplementation() = default;

    void
    setMock(QueueMockRaw& mock) override
    {
        OUTPOST_ASSERT(mock.getTypeInfo() == mInfo,
                       "Can not inject QueueMock<T> into Queue<U> with U != T");
        Mockable<QueueMockRaw, outpost::posix::QueueRaw>::setMock(mock);
    }

    bool
    send(const void* data);

    bool
    sendFromISR(const void* data, bool& hasWokenTask);

    bool
    receive(void* data, outpost::time::Duration timeout);

    bool
    receiveFromISR(void* data, bool& hasWokenTask);

    void
    clear();

    size_t
    getNumberOfPendingMessages();

private:
    outpost::posix::QueueRaw mQueue;
    const std::type_info& mInfo;
};

// ---------------------------------------------------------------------------
template <typename T>
class ReturnElem
{
public:
    explicit ReturnElem(const T& value) : mValue(value)
    {
    }

    template <typename... Args>
    void
    operator()(Args&... args) const
    {
        std::get<0>(std::tie(args...)) = mValue;
    }

private:
    const T mValue;
};

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_QUEUE_MOCK_H
