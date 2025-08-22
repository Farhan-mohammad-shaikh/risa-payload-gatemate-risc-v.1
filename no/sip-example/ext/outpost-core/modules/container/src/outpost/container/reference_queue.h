/*
 * Copyright (c) 2018, 2020, 2023, Jan-Gerd Mess
 * Copyright (c) 2018, Fabian Greif
 * Copyright (c) 2019, 2021, Jan Malburg
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_REFERENCE_QUEUE_H_
#define OUTPOST_UTILS_REFERENCE_QUEUE_H_

#include <outpost/container/shared_buffer.h>
#include <outpost/rtos/queue.h>
#include <outpost/utils/communicator.h>
#include <outpost/utils/expected.h>
#include <outpost/utils/operation_result.h>

namespace outpost
{
namespace container
{
/**
 * \ingroup SharedBuffer
 * \brief Base class of the ReferenceQueue for passing by reference.
 *
 * The standard RTOS/POSIX queues are not capable of handling classes that may not be used as a
 * pointer (e.g. using reference counting), since they use pointers or standard constructors instead
 * of references. Hence, ReferenceQueueBase inherits from outpost::rtos::Queue and adds a wrapper
 * that keeps the references.
 */
template <typename T>
class ReferenceQueueBase : public utils::Sender<T>, public utils::Receiver<T>
{
public:
    /**
     * Default destructor.
     */
    virtual ~ReferenceQueueBase() = default;

    /**
     * \brief Getter function for the number of items currently stored in the queue.
     * \return Returns the number of items in the queue that are ready for receiving.
     */
    virtual uint16_t
    getNumberOfItems() = 0;

    /**
     * \brief Checks whether the queue is currently empty.
     * \return Returns true if there are no elements in the queue waiting to be received, false
     * otherwise.
     */
    virtual bool
    isEmpty() = 0;

    /**
     * \brief Checks whether there is a free slot in the queue.
     * \return Returns true if data can be sent to the queue, false otherwise.
     */
    virtual bool
    isFull() = 0;

protected:
    /**
     * \brief Constructor for a ReferenceQueueBase. May only be called by its derivatives (i.e.
     * ReferenceQueue) \param N Maximum number of elements in the queue
     */
    inline ReferenceQueueBase(size_t N) : mQueue(N)
    {
    }

    outpost::rtos::Queue<size_t> mQueue;
};

/**
 * \ingroup SharedBuffer
 * \brief Implementation of a outpost::rtos::Queue that stores all additional information needed for
 * passing instances of classes that cannot be sent as pointers.
 */
template <typename T, size_t N>
class ReferenceQueue : public ReferenceQueueBase<T>
{
    using OperationResult = utils::OperationResult;

public:
    /**
     * \brief Standard constructor.
     */
    ReferenceQueue() :
        ReferenceQueueBase<T>(N), mEmpty(), mItemsInQueue(0), mLastIndex(0), mPointers{{}}
    {
        for (size_t i = 0; i < N; i++)
        {
            mIsUsed[i] = false;
        }
    }

    virtual ~ReferenceQueue() = default;

    /**
     * \brief Checks whether the queue is currently empty.
     * \return Returns true if there are no elements in the queue waiting to be received, false
     *otherwise.
     */
    bool
    isEmpty() override
    {
        outpost::rtos::MutexGuard lock(mMutex);
        for (size_t i = 0; i < N; i++)
        {
            if (mIsUsed[i])
            {
                return false;
            }
        }
        return true;
    }

    /**
     * \brief Checks whether there is a free slot in the queue.
     * \return Returns true if data can be sent to the queue, false otherwise.
     */
    bool
    isFull() override
    {
        outpost::rtos::MutexGuard lock(mMutex);
        /*for (size_t i = 0; i < N; i++)
        {
            if(!isUsed[i])
                return false;
        }
        return true;*/
        return mItemsInQueue == N;
    }

    /**
     * \brief Send data to the queue.
     * \see ReferenceQueueBase::send(T&)
     * \param data Data to be sent.
     * \return Returns true if data could be sent, false otherwise.
     */
    virtual utils::OperationResult
    send(T& data, time::Duration timeout) override
    {
        if (!mMutex.acquire(timeout))
        {
            return OperationResult::timeout;
        }
        OperationResult res = OperationResult::genericError;
        size_t i = mLastIndex;
        size_t endSearch = (mLastIndex - 1) % N;
        // for (size_t i = 0; i < N; i++)
        do
        {
            if (!mIsUsed[i])
            {
                mPointers[i] = data;
                mIsUsed[i] = true;
                mLastIndex = (i + 1) % N;
                if (ReferenceQueueBase<T>::mQueue.send(i))
                {
                    mItemsInQueue++;
                    res = OperationResult::success;
                }
                else
                {
                    mIsUsed[i] = false;
                    mPointers[i] = mEmpty;
                }
                break;
            }
            i = (i + 1) % N;
        } while (i != endSearch);
        mMutex.release();
        return res;
    }

    /**
     * \brief Receive data from the queue.
     * \see ReferenceQueueBase::receive(T&, outpost::time::Duration)
     *
     * Can be either blocking (timeout > 0) or non-blocking (timeout = 0).
     * \param data Reference for the data to be received.
     * \param timeout Duration for which the caller is willing to wait for incoming data
     * \return Returns true if data was received, false
     * otherwise (e.g. a timeout occured)
     */
    virtual Expected<T, utils::OperationResult>
    receive(time::Duration timeout) override
    {
        size_t index;
        if (ReferenceQueueBase<T>::mQueue.receive(index, timeout))
        {
            outpost::rtos::MutexGuard lock(mMutex);
            T data = mPointers[index];
            mPointers[index] = mEmpty;
            mIsUsed[index] = false;
            mItemsInQueue--;
            return data;
        }
        else
        {
            return outpost::unexpected(OperationResult::timeout);
        }
    }

    /**
     *	\brief Getter function for the number of items currently stored in the queue.
     *	\return Returns the number of items in the queue that are ready for receiving.
     */
    virtual uint16_t
    getNumberOfItems() override
    {
        outpost::rtos::MutexGuard lock(mMutex);
        return mItemsInQueue;
    }

    using utils::Sender<T>::send;
    using utils::Receiver<T>::receive;

private:
    T mEmpty;

    outpost::rtos::Mutex mMutex;

    uint16_t mItemsInQueue;

    size_t mLastIndex;

    T mPointers[N];
    bool mIsUsed[N];
};

using SharedBufferQueueBase = ReferenceQueueBase<SharedBufferPointer>;
template <size_t N>
using SharedBufferQueue = ReferenceQueue<SharedBufferPointer, N>;

}  // namespace container
}  // namespace outpost

#endif /* OUTPOST_UTILS_REFERENCE_QUEUE_H_ */
