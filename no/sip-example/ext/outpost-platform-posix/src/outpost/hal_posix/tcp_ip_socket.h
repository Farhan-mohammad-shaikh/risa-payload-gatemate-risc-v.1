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

#ifndef OUTPOST_POSIX_TCP_IP_SOCKET_H
#define OUTPOST_POSIX_TCP_IP_SOCKET_H

class TcpServer;

#include <netinet/in.h>
#include <sys/socket.h>

#include <outpost/hal/datagram_transport.h>

#include <unordered_map>

namespace outpost
{
namespace posix
{
/*
 * The TCP/IP socket class to connect to a serer, and handle the communication.
 */
class TcpIpSocket
{
public:
    using MaybeBool = outpost::hal::DatagramTransport::MaybeBool;
    using MaybeSize = outpost::hal::DatagramTransport::MaybeSize;
    using MaybeSlice = outpost::hal::DatagramTransport::MaybeSlice;

    /**
     * Constructor with a defined target address.
     *
     * \param[in] address : the address to connect to.
     */
    TcpIpSocket(outpost::hal::IpPortAddress const& address);

    /**
     * Default constructor to use the socket as a client for a TcpServer.
     */
    TcpIpSocket();

    /**
     * Destructor terminates the connection.
     */
    ~TcpIpSocket();

    /**
     * Set up the connection for sending and receiving.
     */
    bool
    connect();

    /**
     * Close the connection.
     */
    void
    close();

    /**
     * Return the target address of the socket i.e. the destination.
     */
    outpost::hal::IpPortAddress
    getAddress() const;

    /**
     * Set a new target address.
     *
     * This action disconnects currently listening clients
     */
    void
    setAddress(const outpost::hal::IpPortAddress& address);

    /**
     * Sets the Differentiated Services Code Point (DSCP) value of the socket.
     */
    outpost::hal::IpErrorCode
    setDscp(const uint8_t dscpValue);

    /**
     * Check if new data is available
     *
     * \retval true   Data is available and can be read via \c receive(...).
     * \retval false  No data available.
     */
    MaybeBool
    isAvailable();

    /**
     * Returns the number of bytes available for the next read, if any.
     * \return  Number of bytes in the buffer
     */
    [[deprecated]] MaybeSize
    getNumberOfBytesAvailable();

    /**
     * Drop all data which is currently in the receive buffer.
     */
    void
    clearReceiveBuffer();

    /**
     * Returns the maximum number of bytes which can be send in one segment.
     *
     * Trying to send a larger amount of data in one \ref send command might
     * cause data loss, or the TCP stack may concatenate them.
     */
    static constexpr size_t
    getMaximumTransferSize()
    {
        return (maxIpPacketSize - maxIpHeaderSize - tcpMaxHeaderSize);
    };

    /**
     * Send data.
     *
     * Tries to send the data ASAP.
     * Uses NoDelay and Urgent flag to tickle the tcp stack.
     *
     * \param data Buffer containing the data to send. Should not exceed \ref
     * getMaximumTransferSize
     * \param timeout Function will return after that time even if
     * not all data sent; default blocking call
     * \return Number of bytes which could be sent,
     * maximal data.getNumberOfElements, or ErrorCode
     * */
    outpost::Expected<size_t, hal::IpErrorCode>
    send(outpost::Slice<const uint8_t> data,
         outpost::time::Duration timeout = outpost::time::Duration::maximum());

    /**
     *  Read data.
     *
     * Reads data until and including the urgent marker.
     * if no urgent data is present, the remaining data or up to \c data.getNumberOfElements() is
     * read.
     *
     * \param [out] data : Buffer to write the received data to
     * \param [in,optional] timeout : Function will return after that time
     * \return contains the sublice of Data on success.
     */
    MaybeSlice
    receive(const outpost::Slice<uint8_t>& data,
            outpost::time::Duration timeout = outpost::time::Duration::maximum());

    static const constexpr size_t tcpMaxHeaderSize = 60;

    // friend TcpServer::accept(TcpIpSocket & socket); // access to mSockFd
    friend class TcpServer;  // access to mSockFd

    static const constexpr size_t maxIpPacketSize = 0xFFFF;
    static const constexpr size_t maxIpHeaderSize = 60;

    typedef in_addr_t AddressKey;
    static in_addr_t
    convertToAddrT(const outpost::hal::IpAddress& address);

    static sockaddr_in
    addressToSockaddr(const outpost::hal::IpPortAddress& address);

    static timeval
    timeoutToTimeval(outpost::time::Duration dur);

    static outpost::hal::IpErrorCode
    convertErrnoToErrorCode(int error);

protected:
    outpost::hal::IpPortAddress mAddress;
    int mSockFd;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_HAL_TC
