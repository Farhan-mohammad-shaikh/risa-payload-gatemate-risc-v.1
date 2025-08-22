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

#ifndef OUTPOST_POSIX_TCP_SERVER_H
#define OUTPOST_POSIX_TCP_SERVER_H

class TcpIpSocket;
#include "tcp_ip_socket.h"

#include <netinet/in.h>
#include <sys/socket.h>

#include <outpost/hal/datagram_transport.h>

#include <unordered_map>

namespace outpost
{
namespace posix
{
/*
 * Opens ports, and listens for connections, returns a TcpSocket upon accept
 */
class TcpServer
{
public:
    using MaybeBool = TcpIpSocket::MaybeBool;

    /**
     * Constructor for the TcpServer object
     *
     * \param[in] address : the address to bind to
     */
    TcpServer(outpost::hal::IpPortAddress const& address);

    /**
     * Destructor also stops listening.
     */
    ~TcpServer();

    /**
     * Bind to the configured IpAddress and listens for incoming connections
     */
    bool
    listen();

    /**
     * Accepts an incoming connection and configures the socket to use it.
     *
     * \param[inout] socket : the socket object being configured to the new connection.
     */
    bool
    accept(TcpIpSocket& socket);

    /**
     * Stops the server. Existing soccets remain unaffected.
     */
    void
    close();

    /**
     * If new connections are waiting to be accepted.
     */
    MaybeBool
    isAvailable();

    /**
     * Return the address of the device
     */
    constexpr outpost::hal::IpPortAddress
    getAddress() const
    {
        return mAddress;
    };

protected:
    int mServerFd;  // the Server Fd, accepted connections get their own Fd
    outpost::hal::IpPortAddress mAddress;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_HAL_TC
