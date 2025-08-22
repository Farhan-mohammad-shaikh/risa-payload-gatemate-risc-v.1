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

#ifndef OUTPOST_RTOS_QUEUE_RAW_H
#define OUTPOST_RTOS_QUEUE_RAW_H

#include <outpost/time/duration.h>

#include <stddef.h>
#include <stdint.h>

#include <typeinfo>

namespace outpost
{
namespace rtos
{
/**
 * \class QueueRaw
 *
 * Can be used to exchange data between different threads.
 */
class QueueRaw
{
public:
    explicit QueueRaw(size_t numberOfItems, size_t itemSize, const std::type_info& info);

    QueueRaw(const QueueRaw& other) = delete;

    QueueRaw&
    operator=(const QueueRaw& other) = delete;

    ~QueueRaw();

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
    void* mImplementation;
};

}  // namespace rtos
}  // namespace outpost

#endif
