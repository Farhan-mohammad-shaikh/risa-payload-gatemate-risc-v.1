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

#include "outpost/hal_posix/can_bus_port.h"

#include <net/if.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

using namespace outpost::posix;

// ---------------------------------------------------------------------------
CanBusPort::CanBusPort(std::string interface) :
    mCanInterface(interface), mSocket(-1), mTimeout(outpost::time::Duration::myriad())
{
}

// ---------------------------------------------------------------------------
CanBusPort::ReturnCode
CanBusPort::open()
{
    mSocket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (mSocket == -1)
    {
        std::cerr << "Error opening socket" << std::endl;
        return ReturnCode::failure;
    }

    // Specify the CAN interface to use
    struct ifreq ifr;
    std::strncpy(ifr.ifr_name, mCanInterface.c_str(), IFNAMSIZ - 1);
    ifr.ifr_name[IFNAMSIZ - 1] = '\0';
    ifr.ifr_ifindex = if_nametoindex(ifr.ifr_name);
    if (!ifr.ifr_ifindex)
    {
        std::cerr << "Error setting interface index" << std::endl;
        ::close(mSocket);
        return ReturnCode::failure;
    }

    // Bind the socket to the CAN interface
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(mSocket, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1)
    {
        std::cerr << "Error binding socket to interface" << std::endl;
        ::close(mSocket);
        return ReturnCode::failure;
    }
    return ReturnCode::success;
}

CanBusPort::ReturnCode
CanBusPort::close()
{
    ::close(mSocket);
    mSocket = -1;
    return ReturnCode::success;
}

CanBusPort::ReturnCode
CanBusPort::read(CanFrame& frame, outpost::time::Duration timeout)
{
    if (timeout != mTimeout)
    {
        mTimeout = timeout;
        updateTimeout();
    }

    struct can_frame basicFrame;
    ssize_t bytesRead = ::recv(mSocket, &basicFrame, sizeof(struct can_frame), MSG_WAITALL);

    if (bytesRead > 0)
    {
        frame = basicFrameToCanFrame(basicFrame);
        return ReturnCode::success;
    }
    else
    {
        return ReturnCode::failure;
    }
}

CanBusPort::ReturnCode
CanBusPort::write(const CanFrame& frame)
{
    struct can_frame basicFrame = canFrameToBasicFrame(frame);
    ssize_t bytesWritten = ::send(mSocket, &basicFrame, sizeof(struct can_frame), 0);
    if (bytesWritten != sizeof(struct can_frame))
    {
        std::cerr << "Error writing CAN frame. Is the link up?" << std::endl;
        return ReturnCode::failure;
    }
    return ReturnCode::success;
}

CanBusPort::ReturnCode
CanBusPort::clearReceiveBuffer()
{
    struct can_frame frame;
    ssize_t bytesRead = 0;
    do
    {
        bytesRead = ::read(mSocket, &frame, sizeof(struct can_frame));
    } while (bytesRead > 0);

    return ReturnCode::success;
}

bool
CanBusPort::isDataAvailable()
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(mSocket, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;  // Immediate timeout

    int ret = select(mSocket + 1, &readSet, NULL, NULL, &timeout);

    if (ret < 0)
    {
        std::cerr << "select error" << std::endl;
    }
    else if (ret > 0)
    {
        return true;
    }
    return false;
}

CanBusPort::CanFrame
CanBusPort::basicFrameToCanFrame(const struct can_frame& basicFrame)
{
    outpost::Slice<const uint8_t> data(basicFrame.data);
    uint8_t numberOfBytesToCopy = std::min(static_cast<uint8_t>(CAN_MAX_DLEN), basicFrame.can_dlc);
    outpost::Slice<const uint8_t> payloadData = data.first(numberOfBytesToCopy);
    CanFrame canFrame(basicFrame.can_id, payloadData);
    return canFrame;
}

struct can_frame
CanBusPort::canFrameToBasicFrame(const CanFrame& canFrame)
{
    struct can_frame basicFrame;
    basicFrame.can_id = canFrame.getId();
    basicFrame.can_dlc = canFrame.getLength();
    outpost::Slice<const uint8_t> payloadData = canFrame.getData();
    size_t numberOfBytesToCopy =
            std::min(static_cast<size_t>(CAN_MAX_DLEN), payloadData.getNumberOfElements());
    for (size_t i = 0; i < numberOfBytesToCopy; i++)
    {
        basicFrame.data[i] = payloadData[i];
    }
    return basicFrame;
}

void
CanBusPort::updateTimeout()
{
    struct timeval tv;
    tv.tv_sec = mTimeout.seconds();
    tv.tv_usec = mTimeout.microseconds()
                 - (mTimeout.seconds() * outpost::time::Duration::millisecondsPerSecond
                    * outpost::time::Duration::microsecondsPerMillisecond);
    setsockopt(mSocket,
               SOL_SOCKET,
               SO_RCVTIMEO,
               reinterpret_cast<const char*>(&tv),
               sizeof tv);  // Zero timeouts immediately
}
