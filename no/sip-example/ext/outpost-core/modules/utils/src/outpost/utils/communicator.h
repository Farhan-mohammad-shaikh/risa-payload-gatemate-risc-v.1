/*
 * Copyright (c) 2021, Jan Malburg
 * Copyright (c) 2021, 2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef MODULES_UTILS_COMMUNICATOR_H_
#define MODULES_UTILS_COMMUNICATOR_H_

#include <outpost/time/duration.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/operation_result.h>

namespace outpost
{
namespace utils
{
template <typename T>
class Receiver
{
public:
    Receiver() = default;
    virtual ~Receiver() = default;

    /**
     * \brief Receives data from the object.
     *
     * Can be either blocking (timeout > 0) or non-blocking (timeout = 0).
     * \param data Reference for the data type to be received.
     * \param timeout Duration for which the caller is willing to wait for incoming data
     * \return Returns true if data was received, false
     * otherwise (e.g. a timeout occured)
     */
    virtual outpost::Expected<T, OperationResult>
    receive(outpost::time::Duration timeout) = 0;

    /**
     * \brief Receives data from the object. With unlimited timeout.
     * Can block
     *
     * \param data Reference for the data type to be received.
     * \return Returns true if data was received, false
     * otherwise
     */
    virtual outpost::Expected<T, OperationResult>
    receive()
    {
        return receive(outpost::time::Duration::myriad());
    }
};

template <typename T>
class Sender
{
public:
    Sender() = default;
    virtual ~Sender() = default;

    /**
     * \brief sends data .
     *
     * Can be either blocking (timeout > 0) or non-blocking (timeout = 0).
     * \param data Reference for the data type to be send.
     * \param timeout Duration for which the caller is willing to wait for sender to be valid
     * \return Returns true if data was send, false
     * otherwise (e.g. a timeout occured)
     */
    virtual OperationResult
    send(T& data, outpost::time::Duration timeout) = 0;

    /**
     * \brief Sends data with unlimited timeout. Can block.
     *
     * \param data Reference for the data type to be send.
     * \return Returns true if data was send, false
     * otherwise
     */
    OperationResult
    send(T& data)
    {
        return send(data, outpost::time::Duration::myriad());
    }
};

}  // namespace utils
}  // namespace outpost

#endif /* MODULES_UTILS_SRC_OUTPOST_UTILS_COMMUNICATOR_H_ */
