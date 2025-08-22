/*
 * Copyright (c) 2024, Felix Passenberg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "tcp_ip_socket.h"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <iostream>

namespace outpost
{
namespace posix
{
TcpIpSocket::TcpIpSocket(outpost::hal::IpPortAddress const& address) :
    mAddress(address), mSockFd(-1)
{
}
TcpIpSocket::TcpIpSocket() : mAddress(), mSockFd(-1)
{
}

TcpIpSocket::~TcpIpSocket()
{
    close();
}

bool
TcpIpSocket::connect()
{
    if (mSockFd > 0)
    {
        // std::cerr << "[TcpSocket] already connected" << std::endl;
        return false;
    }

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_port = htons(mAddress.getPort());
    remote.sin_addr.s_addr =
            (*(reinterpret_cast<uint32_t*>(mAddress.getIpAddress().getArray().data())));

    mSockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mSockFd < 0)
    {
        // std::cerr << "[TcpSocket] could not open socket: " << strerror(errno) << std::endl;
        return false;
    }

    if (::connect(mSockFd, reinterpret_cast<struct sockaddr*>(&remote), sizeof(remote)) < 0)
    {
        // std::cerr << "[TcpSocket] could not connect: " << strerror(errno) << std::endl;
        return false;
    }

    int set = 1;
    if (::setsockopt(mSockFd, IPPROTO_TCP, TCP_NODELAY, &set, sizeof(set)))
    {
        // std::cerr << "[TcpSocket] could not enter NoDelay " << strerror(errno) << std::endl;
        return false;
    }
    int tosValue = IPTOS_LOWDELAY;
    if (::setsockopt(mSockFd, IPPROTO_IP, IP_TOS, &tosValue, sizeof(tosValue)) < 0)
    {
        // std::cerr << "[TcpSocket] could not set TOS" << strerror(errno) << std::endl;
        return false;
    }
    if (::setsockopt(mSockFd, SOL_SOCKET, SO_OOBINLINE, &set, sizeof(set)) < 0)
    {
        // std::cerr << "[TcpSocket] could not set OOB" << strerror(errno) << std::endl;
        // not a fatal error, it's just one way to find packet boundaries
        //  return false;
    }

    return true;
}

void
TcpIpSocket::close()
{
    if (mSockFd >= 0)
    {
        ::shutdown(mSockFd, SHUT_RDWR);
        ::close(mSockFd);
        mSockFd = -1;
    }
}

outpost::hal::IpPortAddress
TcpIpSocket::getAddress() const
{
    return mAddress;
}

void
TcpIpSocket::setAddress(const outpost::hal::IpPortAddress& address)
{
    close();
    mAddress = address;
}

outpost::hal::IpErrorCode
TcpIpSocket::setDscp(const uint8_t dscpValue)
{
    int tosValue = dscpValue << 2;
    if (setsockopt(mSockFd, IPPROTO_IP, IP_TOS, &tosValue, sizeof(tosValue)) < 0)
    {
        return convertErrnoToErrorCode(errno);
    }
    return outpost::hal::IpErrorCode::success;
}

TcpIpSocket::MaybeBool
TcpIpSocket::isAvailable()
{
    static constexpr size_t numberOfRetries = 10;

    OUTPOST_ASSERT(mSockFd >= 0, "Not connected");

    if (mSockFd >= 0)
    {
        struct ::pollfd pollFd;
        pollFd.fd = mSockFd;
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
            return outpost::unexpected(outpost::hal::IpErrorCode::failure);
        }
        else
        {
            return retval == 1;
        }
    }
    else
    {
        return outpost::unexpected(outpost::hal::IpErrorCode::failure);
    }
}

TcpIpSocket::MaybeSize
TcpIpSocket::getNumberOfBytesAvailable()
{
    static constexpr size_t numberOfRetries = 10;

#ifdef __rtems__
    // Unfortunately the "MSG_PEEK | MSG_TRUNC"-trick (see below) does not
    // work on RTEMS. It should return the real size of the packet. Instead it
    // returns the number of read bytes. Which is the size of the buffer
    // (aka.: 0) using ioctl with FIONREAD will return the total number of
    // bytes available. Therefore this will also not work.
    return outpost::unexpected(outpost::hal::IpErrorCode::notImplemented);
#else
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
                len = ::recv(mSockFd, &dummyBuffer[0], 1, MSG_PEEK | MSG_DONTWAIT | MSG_TRUNC);
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
                return outpost::unexpected(outpost::hal::IpErrorCode::failure);
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

outpost::Expected<size_t, outpost::hal::IpErrorCode>
TcpIpSocket::send(outpost::Slice<const uint8_t> data, outpost::time::Duration timeout)
{
    if (mSockFd < 0)
    {
        return outpost::unexpected(outpost::hal::IpErrorCode::permanent);
    }

    struct timeval sendTimeout = timeoutToTimeval(timeout);

    if (setsockopt(mSockFd, SOL_SOCKET, SO_SNDTIMEO, &sendTimeout, sizeof(sendTimeout)) < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }
    int result = 0;
    result = ::send(mSockFd, data.getDataPointer(), data.getNumberOfElements(), MSG_EOR | MSG_OOB);

    if (result < 0)
    {
        // std::cerr << "[TcpSocket] send error" << strerror(errno) << std::endl;
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }
    else if (result == 0)
    {
        return result;
    }
    return result;
}

TcpIpSocket::MaybeSlice
TcpIpSocket::receive(outpost::Slice<uint8_t> const& data, outpost::time::Duration timeout)
{
    if (mSockFd < 0)
    {
        return outpost::unexpected(outpost::hal::IpErrorCode::permanent);
    }

    struct timeval recvTimout = timeoutToTimeval(timeout);

    if (setsockopt(mSockFd, SOL_SOCKET, SO_RCVTIMEO, &recvTimout, sizeof(recvTimout)) < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    const auto result = ::recv(mSockFd, data.getDataPointer(), data.getNumberOfElements(), 0);
    if (result < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    // if the next byte is the urgent byte, it is still part of the "frame", add it.
    int value;
    int error = ioctl(mSockFd, SIOCATMARK, &value);
    if (error < 0)
    {
        return outpost::unexpected(convertErrnoToErrorCode(errno));
    }

    if (value && result < data.getNumberOfElements())
    {
        const auto result2 = ::recv(mSockFd,
                                    data.skipFirst(result).getDataPointer(),
                                    1,
                                    0 /*MSG_OOB -> invalid argument*/);
        if (result2 < 0)
        {
            return outpost::unexpected(convertErrnoToErrorCode(errno));
        }
        OUTPOST_ASSERT(result2 == 1, "TCP received more than the urgent byte");
        return data.first(result + result2);
    }

    return data.first(result);
}

void
TcpIpSocket::clearReceiveBuffer()
{
    size_t tmp;  // Just minimal buffer to discard datagrams
    if (mSockFd >= 0)
    {
        while (recv(mSockFd, &tmp, sizeof(tmp), MSG_DONTWAIT) >= 0)
        {
            // Each read may remove a whole udp packet (irregarding the size)
            // from the receive buffer. If the receive buffer is empty
            // -1 is returned with EWOULDBLOCK.
        }
    }
}

in_addr_t
TcpIpSocket::convertToAddrT(const outpost::hal::IpAddress& address)
{
    in_addr_t ret;
    memcpy(&ret, address.getArray().data(), sizeof(ret));
    return ret;
}

sockaddr_in
TcpIpSocket::addressToSockaddr(const outpost::hal::IpPortAddress& address)
{
    sockaddr_in sockAddress;
    memset(&sockAddress, 0, sizeof(sockAddress));
    sockAddress.sin_family = AF_INET;
    sockAddress.sin_port = htons(address.getPort());
    sockAddress.sin_addr.s_addr = convertToAddrT(address.getIpAddress());
    return sockAddress;
}

struct timeval
TcpIpSocket::timeoutToTimeval(outpost::time::Duration dur)
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

outpost::hal::IpErrorCode
TcpIpSocket::convertErrnoToErrorCode(int error)
{
    switch (error)
    {
        case EAGAIN:
        case ENOMEM:
        case EBUSY: return outpost::hal::IpErrorCode::temporary;
        case EINVAL:
        case EFAULT:
        case EMSGSIZE: return outpost::hal::IpErrorCode::permanent;
        default: return outpost::hal::IpErrorCode::unknown;
    }
}

}  // namespace posix
}  // namespace outpost
