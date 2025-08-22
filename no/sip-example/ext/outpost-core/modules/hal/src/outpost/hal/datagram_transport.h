/*
 * Copyright (c) 2017, 2021, Jan Sommer
 * Copyright (c) 2017-2018, Fabian Greif
 * Copyright (c) 2017, Ulmer, Moritz Christian
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2021, Sommer, Jan
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023-2024, Pieper, Pascal
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Meß, Jan-Gerd
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Passenberg, Felix Constantin
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_DATAGRAM_TRANSPORT_H
#define OUTPOST_HAL_DATAGRAM_TRANSPORT_H

#include <outpost/base/slice.h>
#include <outpost/container/fixed_size_array.h>
#include <outpost/time/duration.h>
#include <outpost/utils/expected.h>

#include <array>

namespace outpost
{
namespace hal
{

class IpAddress
{
public:
    /**
     * Default constructor.
     *
     * Set the Ip address to zero.
     */
    constexpr IpAddress() : mIpAddress{{0, 0, 0, 0}}
    {
    }

    /**
     * Constructor
     *
     * \param ipAddress IPv4 Address provided in host-byte-order
     */
    explicit constexpr IpAddress(std::array<uint8_t, 4> ipAddress) : mIpAddress(ipAddress)
    {
    }

    /**
     * Constructor
     *
     * Allows to provide the IP-Address more conveniently, e.g.:
     * \code
     * IpAddress myIp(192, 168, 0, 1);
     * \endcode
     *
     * \param byte1 First byte (left most) of an IPv4 Address
     * \param byte2 Second  byte of an IPv4 Address
     * \param byte3 Third byte of an IPv4 Address
     * \param byte4 Fourth byte (right most) of an IPv4 Address
     */
    constexpr IpAddress(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) :
        // store the IP address in network-byte-order
        mIpAddress{{byte1, byte2, byte3, byte4}}
    {
    }

    /**
     * Returns the internal byte array holding the ip-address in
     * network-byte-order.
     *
     * \return Array holding the IP-Address
     */
    constexpr std::array<uint8_t, 4>
    getArray() const
    {
        return mIpAddress;
    }

    constexpr uint8_t
    operator[](size_t index) const
    {
        return mIpAddress[index];
    }

    inline bool
    operator==(const IpAddress& other) const
    {
        return (mIpAddress == other.mIpAddress);
    }

    inline bool
    operator!=(const IpAddress& other) const
    {
        return !operator==(other);
    }

    constexpr bool
    isMulticast() const
    {
        /*
         * "The multicast addresses are in the range 224.0.0.0 through 239.255.255.255."
         * https://www.iana.org/assignments/multicast-addresses/multicast-addresses.xhtml
         */
        return mIpAddress[0] >= 224 && mIpAddress[0] <= 239;
    }

private:
    std::array<uint8_t, 4> mIpAddress;
};

/**
 * Representation of an datagram address composed of an IP and a port number.
 */
class IpPortAddress
{
public:
    /**
     * Constructor
     */
    constexpr IpPortAddress() : mIpAddress(), mPort(0)
    {
    }

    constexpr IpPortAddress(const IpAddress& ip, uint16_t port) : mIpAddress(ip), mPort(port)
    {
    }

    /**
     * \return The port in host-byte-order
     */
    constexpr uint16_t
    getPort() const
    {
        return mPort;
    }

    constexpr IpAddress
    getIpAddress() const
    {
        return mIpAddress;
    }

    inline bool
    operator==(const IpPortAddress& other) const
    {
        return (mIpAddress == other.mIpAddress) && (mPort == other.mPort);
    }

    inline bool
    operator!=(const IpPortAddress& other) const
    {
        return !operator==(other);
    }

protected:
    IpAddress mIpAddress;
    uint16_t mPort;
};

enum class IpErrorCode
{
    success = 0,  // should never happen, but added because zero is always "success"
    temporary,
    permanent,
    notImplemented,
    failure,
    unknown
};

/*
 * Interface class for datagram based IP communication (e.g. UDP)
 *
 * This class provides a generic interface for network communication
 * using the internet protocol (ATM only version 4). It corresponds to
 * the Transport Layer (Layer 4) of the OSI model with the focus on
 * a packet/datagram style of communication (e.g. UDP).
 *
 */
class DatagramTransport
{
public:
    using IpAddress = outpost::hal::IpAddress;
    using Address = outpost::hal::IpPortAddress;
    using ErrorCode = outpost::hal::IpErrorCode;
    using MaybeBool = outpost::Expected<bool, ErrorCode>;
    using MaybeSize = outpost::Expected<size_t, ErrorCode>;
    using MaybeSlice = outpost::Expected<outpost::Slice<const uint8_t>, ErrorCode>;

    /**
     * Destructor
     */
    virtual ~DatagramTransport();

    /**
     * Set up the connection for sending and receiving
     */
    virtual bool
    connect() = 0;

    /**
     * Close the Ethernet device
     */
    virtual void
    close() = 0;

    /**
     * Return the address of the device
     */
    virtual Address
    getAddress() const = 0;

    /**
     * Sets the address of the device and disconnects listening clients
     *
     * Will reset the connection causing all active calls to \ref sendTo and
     * \ref receiveFrom to return immediately. The internal address is set to
     * newAddress. The connection is not initialized automatically. A call to
     * \ref connect is needed before any data can be received or send again.
     *
     * \param newAddress The new address to use
     */
    virtual void
    setAddress(const Address& newAddress) = 0;

    /**
     * Check if a new datagram is available
     *
     * \retval true, if data is available and can be read via receiveFrom(...).
     * \retval false, if no data available.
     * \retval ErrorCode, if the availability of the socket could not be
     *      retrieved
     */
    virtual MaybeBool
    isAvailable() = 0;

    /**
     * Returns the number of bytes of the next available datagram, if any.
     *
     * \warning This method can not be implemented on all POSIX systems. If
     *      your project needs to be portable, don't use this method.
     *      If you want to check if a received messages was truncated please
     *      use \c recvFrom(...) as recommended in the comment of the
     *      method. If you want to check if the socket is available (I.e.:
     *      that there is at least one packet) please use \c isAvailable().
     *
     * \retval Number of bytes of the next available datagram
     * \retval ErrorCode, if the size could not be retrieved
     */
    [[deprecated]] virtual MaybeSize
    getNumberOfBytesAvailable() = 0;

    /**
     * Returns the maximum number of bytes which can be send in one datagram
     *
     * Trying to send a larger amount of data in one \ref sendTo command might
     * cause data loss.
     */
    virtual size_t
    getMaximumDatagramSize() const = 0;

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
           const Address& address,
           outpost::time::Duration timeout =
                   std::numeric_limits<outpost::time::Duration>::max()) = 0;

    /**
     * \brief Add socket to the multicast group
     *
     * After adding the socket to the multicast group, udp packages send to the
     * multicast address can be retrieved by using `receiveFrom()`.
     *
     * \param address
     *      Address of the multicast group
     */
    virtual ErrorCode
    joinMulticastGroup(const IpAddress& address) = 0;

    /**
     * \brief Read a datagram received from a remote address with timeout
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
     * \note If the address is a multicast address make sure to call
     *      `joinMulticastGroup(..)` beforehand.
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
                Address& address,
                outpost::time::Duration timeout =
                        std::numeric_limits<outpost::time::Duration>::max()) = 0;

    /**
     * Drop all datagrams which are currently in the receive buffer
     */
    virtual void
    clearReceiveBuffer() = 0;
};

}  // namespace hal
}  // namespace outpost

#endif  // OUTPOST_HAL_DATAGRAM_TRANSPORT_H
