/*
 * Copyright (c) 2017, 2021, Jan Sommer
 * Copyright (c) 2017, Moritz Ulmer
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2021, 2023, Jan-Gerd Mess
 * Copyright (c) 2021, Moyano, Gabriel
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2023, Mess, Jan-Gerd
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Christof Efkemann
 * Copyright (c) 2023, cold_ei
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "udp_socket.h"

#include <arpa/inet.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <outpost/base/testing_assert.h>

#include <iostream>

namespace outpost
{
namespace posix
{
UdpSocket::UdpSocket(const DatagramTransport::Address& address) :
    mAddress(address), mEnabledMulticastSend(false), mSockfd(-1)
{
}

UdpSocket::~UdpSocket()
{
    close();
}

bool
UdpSocket::connect()
{
    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(mAddress.getPort());
    remote.sin_addr.s_addr = INADDR_ANY;

    mSockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (mSockfd < 0)
    {
        // std::cerr << "[UdpSocket] could not open socket: " << strerror(errno) << std::endl;
        return false;
    }
    if (::bind(mSockfd, reinterpret_cast<struct sockaddr*>(&remote), sizeof(remote)) < 0)
    {
        // std::cerr << "[UdpSocket] ::bind : " <<  strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void
UdpSocket::close()
{
    if (mSockfd >= 0)
    {
        ::shutdown(mSockfd, SHUT_RDWR);
        ::close(mSockfd);
        mSockfd = -1;
    }
    mEnabledMulticastSend = false;
}

outpost::hal::DatagramTransport::Address
UdpSocket::getAddress() const
{
    return mAddress;
}

void
UdpSocket::setAddress(const outpost::hal::DatagramTransport::Address& address)
{
    close();
    mAddress = address;
}

outpost::hal::DatagramTransport::ErrorCode
UdpSocket::setDscp(const uint8_t dscpValue)
{
    int tosValue = dscpValue << 2;
    if (setsockopt(mSockfd, IPPROTO_IP, IP_TOS, &tosValue, sizeof(tosValue)) < 0)
    {
        return convertErrnoToErrorCode(errno);
    }
    return ErrorCode::success;
}

UdpSocket::MaybeBool
UdpSocket::isAvailable()
{
    static constexpr size_t numberOfRetries = 10;

    OUTPOST_ASSERT(mSockfd >= 0, "Not connected");

    if (mSockfd >= 0)
    {
        struct ::pollfd pollFd;
        pollFd.fd = mSockfd;
        pollFd.events = POLLIN | POLLPRI;
        pollFd.revents = 0;
        int timeout = 0;

        int retval;
        bool again = true;

        for (size_t i = 0U; i < numberOfRetries && again; i++)
        {
            retval = ::poll(&pollFd, 1, timeout);
            again = (retval == -1) && (errno == EINTR);
        }

        if (retval == -1)
        {
            return outpost::unexpected(ErrorCode::failure);
        }
        else
        {
            return retval == 1;
        }
    }
    else
    {
        return outpost::unexpected(ErrorCode::failure);
    }
}

UdpSocket::MaybeSize
UdpSocket::getNumberOfBytesAvailable()
{
#ifdef __rtems__
    // Unfortunately the "MSG_PEEK | MSG_TRUNC"-trick (see below) does not
    // work on RTEMS. It should return the real size of the packet. Instead it
    // returns the number of read bytes. Which is the size of the buffer
    // (aka.: 0) using ioctl with FIONREAD will return the total number of
    // bytes available. Therefore this will also not work.
    return outpost::unexpected(ErrorCode::notImplemented);
#else
    static constexpr size_t numberOfRetries = 10;
    const auto maybeAvailable = isAvailable();
    if (maybeAvailable.has_value())
    {
        if (*maybeAvailable)
        {
            uint8_t dummyBuffer[1] = {0};

            ssize_t len;
            bool again = true;
            for (size_t i = 0U; i < numberOfRetries && again; i++)
            {
                // `recv` can be used with the flag `MSG_TRUNC` to get the size
                // of the next packet. Instead of returning the number of bytes
                // read it will return the number of bytes of the packet, even
                // if the buffer is smaller. `MSG_PEEK` will prevent that the
                // packet will be removed from the queue. `MSG_DONTWAIT` not
                // necessary since the socket is available but can not hurt to
                // make it explicit non blocking.
                // Unfortunately `MSG_TRUNC` does not work on all POSIX/Linux
                // implementations / versions. Therfore this method
                // (`getNumberOfAvailableBytes()`) should not be used.
                len = ::recv(mSockfd, &dummyBuffer[0], 1, MSG_PEEK | MSG_DONTWAIT | MSG_TRUNC);
                again = (len == -1) && (errno == EINTR);
            }

            if (len >= 0)
            {
                return len;
            }
            else
            {
                // if this assertion does not hold it is very likely that MSG_TRUNC
                // is not supported on your system. Unfortunately this is the case
                // if you run the socket under WSL
                OUTPOST_ASSERT(errno != EINVAL, "MSG_TRUNC may not be supported");
                return outpost::unexpected(ErrorCode::failure);
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return outpost::unexpected(maybeAvailable.error());
    }
#endif
}

size_t
UdpSocket::getMaximumDatagramSize() const
{
    return (UdpSocket::maxIpPacketSize - UdpSocket::maxIpHeaderSize - udpHeaderSize);
}

UdpSocket::MaybeSize
UdpSocket::sendTo(outpost::Slice<const uint8_t> data,
                  const DatagramTransport::Address& address,
                  outpost::time::Duration timeout)
{
    if (mSockfd < 0)
    {
        return outpost::unexpected(ErrorCode::permanent);
    }

    if (address.getIpAddress().isMulticast() && !mEnabledMulticastSend)
    {
        struct in_addr localInterface;
        memset(&localInterface, 0, sizeof(struct in_addr));
        const auto ipArr = mAddress.getIpAddress().getArray();
        localInterface.s_addr =
                *reinterpret_cast<const uint32_t*>(ipArr.data());  // Data already in network order
        if (setsockopt(
                    mSockfd, IPPROTO_IP, IP_MULTICAST_IF, &localInterface, sizeof(localInterface))
            < 0)
        {
            // std::cerr << "[UdpSocket] could not set sockopt to IP_MULTICAST_IF: " <<
            // strerror(errno) << std::endl;
            return outpost::unexpected(ErrorCode::permanent);
        }
        mEnabledMulticastSend = true;
    }

    sockaddr_in receiver = addressToSockaddr(address);
    struct timeval sendTimeout = timeoutToTimeval(timeout);

    if (setsockopt(mSockfd, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, sizeof(sendTimeout)) < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    const auto result = sendto(mSockfd,
                               data.getDataPointer(),
                               data.getNumberOfElements(),
                               0,
                               reinterpret_cast<struct sockaddr*>(&receiver),
                               sizeof(receiver));
    if (result < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }
    return result;
}

UdpSocket::ErrorCode
UdpSocket::joinMulticastGroup(const IpAddress& address)
{
    if (mSockfd < 0)
    {
        return ErrorCode::permanent;
    }

    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = convertToAddrT(address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(mSockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
    {
        // std::cerr << "[UdpSocket] could not ADD_MEMBERSHIP to multicast group: " <<
        // strerror(errno) << std::endl;
        return convertErrnoToErrorCode(errno);
    }
    return ErrorCode::success;
}

UdpSocket::MaybeSlice
UdpSocket::receiveFrom(const outpost::Slice<uint8_t>& data,
                       DatagramTransport::Address& address,
                       outpost::time::Duration timeout)
{
    if (mSockfd < 0)
    {
        return outpost::unexpected(ErrorCode::permanent);
    }

    struct timeval recvTimeout = timeoutToTimeval(timeout);
    sockaddr_in from;
    socklen_t addrLen = sizeof(from);

    if (setsockopt(mSockfd, SOL_SOCKET, SO_RCVTIMEO, &recvTimeout, sizeof(recvTimeout)) < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    const auto result = ::recvfrom(mSockfd,
                                   data.getDataPointer(),
                                   data.getNumberOfElements(),
                                   0,
                                   reinterpret_cast<struct sockaddr*>(&from),
                                   &addrLen);
    if (result < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    // Copy the remote terminal address to "address"
    const uint8_t* bytes = reinterpret_cast<uint8_t*>(&from.sin_addr.s_addr);
    DatagramTransport::IpAddress ip(bytes[0], bytes[1], bytes[2], bytes[3]);
    address = DatagramTransport::Address(ip, ntohs(from.sin_port));

    return data.first(result);
}

void
UdpSocket::clearReceiveBuffer()
{
    size_t tmp;  // Just minimal buffer to discard datagrams
    if (mSockfd >= 0)
    {
        while (recv(mSockfd, &tmp, sizeof(tmp), MSG_DONTWAIT) >= 0)
        {
            // Each read may remove a whole udp packet (irregarding the size)
            // from the receive buffer. If the receive buffer is empty
            // -1 is returned with EWOULDBLOCK.
        }
    }
}

in_addr_t
UdpSocket::convertToAddrT(const DatagramTransport::IpAddress& address)
{
    in_addr_t ret;
    memcpy(&ret, address.getArray().data(), sizeof(ret));
    return ret;
}

sockaddr_in
UdpSocket::addressToSockaddr(const DatagramTransport::Address& address)
{
    sockaddr_in sockAddress;
    memset(&sockAddress, 0, sizeof(sockAddress));
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_port = htons(address.getPort());
    sockAddress.sin_addr.s_addr = convertToAddrT(address.getIpAddress());
    return sockAddress;
}

struct timeval
UdpSocket::timeoutToTimeval(outpost::time::Duration dur)
{
    timeval timeOut;
    if (dur == outpost::time::Duration::maximum())
    {
        // A blocking call in posix means a timeout of 0
        timeOut.tv_sec = 0;
        timeOut.tv_usec = 0;
    }
    else if (dur > outpost::time::Duration::zero())
    {
        timeOut.tv_sec = dur.seconds();
        timeOut.tv_usec = dur.microseconds() - outpost::time::Seconds(dur.seconds()).microseconds();
    }
    else
    {
        // if the timeout is negative or zero
        // cap it to 1 us to still have a
        // non-blocking call
        timeOut.tv_sec = 0;
        timeOut.tv_usec = 1;
    }
    return timeOut;
}

outpost::hal::DatagramTransport::ErrorCode
UdpSocket::convertErrnoToErrorCode(int error)
{
    switch (error)
    {
        case EAGAIN:
        case ENOMEM:
        case EBUSY: return ErrorCode::temporary;
        case EINVAL:
        case EFAULT:
        case EMSGSIZE: return ErrorCode::permanent;
        default: return ErrorCode::unknown;
    }
}

}  // namespace posix
}  // namespace outpost
