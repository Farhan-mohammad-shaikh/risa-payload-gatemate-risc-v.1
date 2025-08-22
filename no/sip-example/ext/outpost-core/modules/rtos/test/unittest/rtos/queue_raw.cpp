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

#include "outpost/rtos/queue_raw.h"

#include "unittest/rtos/queue_mock.h"

using namespace outpost::rtos;

// ---------------------------------------------------------------------------
QueueRaw::QueueRaw(size_t numberOfItems, size_t itemSize, const std::type_info& info) :
    mImplementation(nullptr)
{
    auto* queue = new unittest::rtos::QueueImplementation(this, numberOfItems, itemSize, info);
    mImplementation = queue;
}

QueueRaw::~QueueRaw()
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    delete queue;
    mImplementation = nullptr;
}

// ---------------------------------------------------------------------------
bool
QueueRaw::send(const void* data)
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->send(data);
}

bool
QueueRaw::sendFromISR(const void* data, bool& hasWokenTask)
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->sendFromISR(data, hasWokenTask);
}

bool
QueueRaw::receive(void* data, outpost::time::Duration timeout)
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->receive(data, timeout);
}

bool
QueueRaw::receiveFromISR(void* data, bool& hasWokenTask)
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->receiveFromISR(data, hasWokenTask);
}

void
QueueRaw::clear()
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->clear();
}

size_t
QueueRaw::getNumberOfPendingMessages()
{
    auto* queue = reinterpret_cast<unittest::rtos::QueueImplementation*>(mImplementation);
    return queue->getNumberOfPendingMessages();
}
