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

#include "unittest/rtos/queue_mock.h"

using namespace outpost::posix;
using namespace unittest::rtos;

// ---------------------------------------------------------------------------
QueueMockRaw::QueueMockRaw(MockMode mode, const std::type_info& info) :
    Mock<QueueImplementation>(mode), mInfo(info)
{
}

const std::type_info&
QueueMockRaw::getTypeInfo() const
{
    return mInfo;
}

// ---------------------------------------------------------------------------
QueueImplementation::QueueImplementation(outpost::rtos::QueueRaw* queue,
                                         size_t numberOfItems,
                                         size_t itemSize,
                                         const std::type_info& info) :
    Mockable<QueueMockRaw, outpost::posix::QueueRaw>(
            this, reinterpret_cast<intptr_t>(queue), &mQueue),
    mQueue(numberOfItems, itemSize),
    mInfo(info)
{
}

// ---------------------------------------------------------------------------
bool
QueueImplementation::send(const void* data)
{
    std::function<bool()> mockFunction = std::bind(&QueueMockRaw::sendRaw, mMock, data);
    std::function<bool()> realFunction = std::bind(&QueueRaw::send, &mQueue, data);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
QueueImplementation::sendFromISR(const void* data, bool& hasWokenTask)
{
    std::function<bool()> mockFunction =
            std::bind(&QueueMockRaw::sendFromISRRaw, mMock, data, hasWokenTask);
    std::function<bool()> realFunction =
            std::bind(&QueueRaw::sendFromISR, &mQueue, data, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
QueueImplementation::receive(void* data, outpost::time::Duration timeout)
{
    std::function<bool()> mockFunction = std::bind(&QueueMockRaw::receiveRaw, mMock, data, timeout);
    std::function<bool()> realFunction = std::bind(&QueueRaw::receive, &mQueue, data, timeout);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

bool
QueueImplementation::receiveFromISR(void* data, bool& hasWokenTask)
{
    std::function<bool()> mockFunction =
            std::bind(&QueueMockRaw::receiveFromISRRaw, mMock, data, hasWokenTask);
    std::function<bool()> realFunction =
            std::bind(&QueueRaw::receiveFromISR, &mQueue, data, hasWokenTask);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

void
QueueImplementation::clear()
{
    std::function<void()> mockFunction = std::bind(&QueueMockRaw::clearRaw, mMock);
    std::function<void()> realFunction = std::bind(&QueueRaw::clear, &mQueue);
    return handleMethodCall(mMock, mockFunction, realFunction);
}

size_t
QueueImplementation::getNumberOfPendingMessages()
{
    std::function<size_t()> mockFunction =
            std::bind(&QueueMockRaw::getNumberOfPendingMessagesRaw, mMock);
    std::function<size_t()> realFunction =
            std::bind(&QueueRaw::getNumberOfPendingMessages, &mQueue);
    return handleMethodCall(mMock, mockFunction, realFunction);
}
