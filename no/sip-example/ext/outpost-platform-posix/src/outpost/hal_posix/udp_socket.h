/*
 * Copyright (c) 2017, Jan Sommer
 * Copyright (c) 2017, Moritz Ulmer
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2021, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
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

#ifndef OUTPOST_POSIX_UDP_SOCKET_H
#define OUTPOST_POSIX_UDP_SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>

#include <outpost/hal/datagram_transport.h>

#include <unordered_map>

namespace outpost
{
namespace posix
{
/**
 * Implements the DatagramTransport interface for UDP using POSIX sockets
 */
class UdpSocket : public outpost::hal::DatagramTransport
{
public:
    explicit UdpSocket(const DatagramTransport::Address& address);

    UdpSocket(const UdpSocket&) = delete;

    UdpSocket&
    operator=(const UdpSocket&) = delete;

    virtual ~UdpSocket();

    /**
     * Set up the connection for sending and receiving
     */
    virtual bool
    connect() override;

    /**
     * Close the connection
     */
    virtual void
    close() override;

    /**
     * Return the address of the device
     */
    virtual DatagramTransport::Address
    getAddress() const override;

    /**
     * Sets the address of the device.
     *
     * This action disconnects currently listening clients
     */
    virtual void
    setAddress(const DatagramTransport::Address& address) override;

    /**
     * Sets the Differentiated Services Code Point (DSCP) value of the socket.
     */
    outpost::hal::DatagramTransport::ErrorCode
    setDscp(const uint8_t dscpValue);

    /**
     * Check if a new datagram is available
     *
     * \retval true, Data is available and can be read via receiveFrom(...).
     * \retval false, No data available.
     * \retval ErrorCode, if the availability of the socket could not be
     *      retrieved
     */
    virtual MaybeBool
    isAvailable() override;

    /**
     * Returns the number of bytes of the next available datagram, if any.
     *
     * \warning This method can not be implemented on all POSIX systems. If
     *      your project needs to be portable, don't use this method.
     *      If you want to check if a received messages was truncated please
     *      use \c recvFrom(...) as recommended in the comment of the
     *      method. If you want to check if the socket is available please
     *      use \c isAvailable().
     *
     * \retval Number of bytes of the next available datagram
     * \retval ErrorCode, if the size could not be retrieved
     */
    [[deprecated("Method can't be implemented on all systems")]] virtual MaybeSize
    getNumberOfBytesAvailable() override;

    /**
     * Returns the maximum number of bytes which can be send in one datagram
     *
     * Trying to send a larger amount of data in one \ref sendTo command might
     * cause data loss.
     */
    virtual size_t
    getMaximumDatagramSize() const override;

    virtual ErrorCode
    joinMulticastGroup(const IpAddress& address) override;

    /**
     * Send a datagram with \p data to \p address
     *
     * \param data
     *      Buffer containing the data to send. Should not exceed
     *      \ref getMaximumDatagramSize.
     * \param address
     *      Address of the remote terminal to send the data to.
     * \param timeout
     *      Function will return after that time even is not all data sent;
     *      default blocking call.
     *
     * \return
     *      Number of bytes which could be sent, maximal
     *      \c data.getNumberOfElements() or ErrorCode
     */
    virtual MaybeSize
    sendTo(outpost::Slice<const uint8_t> data,
           const DatagramTransport::Address& address,
           outpost::time::Duration timeout = outpost::time::Duration::maximum()) override;

    /**
     * Read a datagram received from a remote address with timeout.
     *
     * Reads the next datagram received or waits until timeout occurs. If
     * \c data.getNumberOfElements() is smaller than the length of the next
     * datagram, only \c data.getNumberOfElements() bytes will be read back and
     * the remaining byte of the datagram will be **lost**.
     *
     * To check whether the message was truncated, one can increase the read
     * buffer by one byte. If the size of the returned slice is equal to
     * \c data.getNumberOfElements(), (i.e. the wanted bytes plus one), the
     * message was truncated.
     *
     * \param data
     *      Buffer to write the received data to
     * \param address
     *      Will contain the address from which the datagram was received, if
     *      available.
     *
     * \return Slice to the received data, limited by
     *      \c data.getNumberOfElements() or ErrorCode
     */
    virtual MaybeSlice
    receiveFrom(const outpost::Slice<uint8_t>& data,
                DatagramTransport::Address& address,
                outpost::time::Duration timeout = outpost::time::Duration::maximum()) override;

    /**
     * Drop all datagrams which are currently in the receive buffer
     */
    virtual void
    clearReceiveBuffer() override;

    static const constexpr size_t maxIpPacketSize = 0xFFFF;
    static const constexpr size_t maxIpHeaderSize = 60;
    static const constexpr size_t udpHeaderSize = 8;

    static in_addr_t
    convertToAddrT(const DatagramTransport::IpAddress& address);

    static sockaddr_in
    addressToSockaddr(const DatagramTransport::Address& address);

    static timeval
    timeoutToTimeval(outpost::time::Duration dur);

    static outpost::hal::DatagramTransport::ErrorCode
    convertErrnoToErrorCode(int error);

private:
    DatagramTransport::Address mAddress;

    bool mEnabledMulticastSend;
    int mSockfd;
};

}  // namespace posix
}  // namespace outpost

#endif  // OUTPOST_HAL_UDP_SOCKET_H
