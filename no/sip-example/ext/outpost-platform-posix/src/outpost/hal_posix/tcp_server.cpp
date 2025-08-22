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

#include "tcp_server.h"

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iomanip>
#include <iostream>

namespace outpost
{
namespace posix
{
TcpServer::TcpServer(outpost::hal::IpPortAddress const& address) : mAddress(address)
{
}

TcpServer::~TcpServer()
{
    close();
}

bool
TcpServer::listen()
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(mAddress.getPort());
    // local.sin_addr.s_addr = INADDR_ANY;
    local.sin_addr.s_addr =
            (*(reinterpret_cast<uint32_t*>(mAddress.getIpAddress().getArray().data())));

    mServerFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (mServerFd < 0)
    {
        // std::cerr << "[TcpServer] could not open socket: " << strerror(errno) << std::endl;
        return false;
    }

    if (::bind(mServerFd, reinterpret_cast<struct sockaddr*>(&local), sizeof(local)) < 0)
    {
        // std::cerr << "[TcpServer] could not bind: " << strerror(errno) << std::endl;
        // std::cerr << "[TcpServer] port: " << mAddress.getPort() << std::endl;
        return false;
    }

    if (::listen(mServerFd, 1) < 0)
    {
        // std::cerr << "[TcpServer] could not listen: " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool
TcpServer::accept(TcpIpSocket& socket)
{
    struct sockaddr remote;
    socklen_t len = sizeof(remote);
    socket.mSockFd = ::accept(mServerFd, &remote, &len);
    if (socket.mSockFd <= 0)
    {
        // std::cerr << "[TcpServer] ::accept error: " << strerror(errno) << std::endl;
        return false;
    }
    sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&remote);
    hal::IpAddress ipAddr;
    memcpy(&ipAddr, &(addr->sin_addr), 4);
    uint16_t port = (addr->sin_port >> 8) + ((addr->sin_port & 0xff) << 8);
    socket.mAddress = hal::IpPortAddress(ipAddr, port);

    int set = 1;
    if (::setsockopt(socket.mSockFd, IPPROTO_TCP, TCP_NODELAY, &set, sizeof(set)))
    {
        // std::cerr << "[TcpServer] could not enter NoDelay " << strerror(errno) << std::endl;
        return false;
    }
    int tosValue = IPTOS_LOWDELAY;
    if (::setsockopt(socket.mSockFd, IPPROTO_IP, IP_TOS, &tosValue, sizeof(tosValue)) < 0)
    {
        // std::cerr << "[TcpServer] could not set TOS" << strerror(errno) << std::endl;
        return false;
    }

    if (::setsockopt(socket.mSockFd, SOL_SOCKET, SO_OOBINLINE, &set, sizeof(set)) < 0)
    {
        // std::cerr << "[TcpServer] could not set OOB" << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

void
TcpServer::close()
{
    if (mServerFd >= 0)
    {
        ::shutdown(mServerFd, SHUT_RDWR);
        ::close(mServerFd);
        mServerFd = -1;
    }
}

TcpServer::MaybeBool
TcpServer::isAvailable()
{
    // TODO change to see if awaiting connections
    static constexpr size_t numberOfRetries = 10;

    OUTPOST_ASSERT(mServerFd >= 0, "Not listening");

    if (mServerFd >= 0)
    {
        struct ::pollfd pollFd;
        pollFd.fd = mServerFd;
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

}  // namespace posix
}  // namespace outpost
