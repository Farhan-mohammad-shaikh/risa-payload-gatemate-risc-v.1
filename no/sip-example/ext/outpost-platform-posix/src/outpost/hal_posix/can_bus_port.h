/*
 * Copyright (c) 2023, Janosch Reinking
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_POSIX_CAN_BUS_H
#define OUTPOST_HAL_POSIX_CAN_BUS_H

#include <linux/can.h>

#include <outpost/hal/can_bus.h>

#include <string>

namespace outpost
{
namespace posix
{

/**
 * \class CanBusPort
 *
 *  Implements the \c CanBus for linux.
 */
class CanBusPort : public outpost::hal::CanBus
{
public:
    CanBusPort(std::string interface);

    ReturnCode
    open() override;

    ReturnCode
    close() override;

    ReturnCode
    read(CanFrame& frame,
         outpost::time::Duration timeout = outpost::time::Duration::myriad()) override;

    ReturnCode
    write(const CanFrame& frame) override;

    ReturnCode
    clearReceiveBuffer() override;

    bool
    isDataAvailable() override;

    static CanFrame
    basicFrameToCanFrame(const can_frame& basicFrame);

    static struct can_frame
    canFrameToBasicFrame(const CanFrame& canFrame);

private:
    void
    updateTimeout();

    std::string mCanInterface;
    int mSocket;
    outpost::time::Duration mTimeout;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_HAL_POSIX_CAN_BUS_H