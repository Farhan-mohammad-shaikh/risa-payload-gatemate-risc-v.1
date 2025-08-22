/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SMPC_TOPIC_RAW_H
#define OUTPOST_SMPC_TOPIC_RAW_H

#include <outpost/container/implicit_list.h>
#include <outpost/rtos/mutex.h>

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace smpc
{
// forward declaration
class SubscriptionRaw;

/**
 * Non type-safe %Topic.
 *
 * Data can be published under a predefined topic. Any number of
 * subscribers can connect to that topic to get notified if new
 * data is published.
 *
 * There is also a type-safe version of this Publisher<>%Subscriber
 * protocol called outpost::smpc::Topic available. Use that version if
 * you want to exchange data with a known length. If in doubt use the
 * type-safe version!
 *
 * \see     outpost::smpc::Topic
 * \ingroup smpc
 * \author  Fabian Greif
 */
class TopicRaw : protected ImplicitList<TopicRaw>
{
public:
    // Needed to allow SubscriptionRaw() to append itself to the
    // subscription list
    friend class SubscriptionRaw;
    friend class ImplicitList<TopicRaw>;

    /**
     * Create a new raw topic.
     */
    TopicRaw();

    /**
     * Destroy the topic.
     *
     * \warning
     *         The destruction and creation of topics during the normal
     *         runtime is not thread-safe. If topics need to be
     *         destroyed outside the initialization of the application
     *         it is necessary to hold all other threads which
     *         might also create or destroy topics and/or subscriptions.
     */
    ~TopicRaw();

    /**
     * Publish new data.
     *
     * Forwards the pointer to all connected subscribers.
     */
    void
    publish(const void* message, size_t length);

private:
    // disable copy constructor
    TopicRaw(const TopicRaw&);

    // disable assignment operator
    TopicRaw&
    operator=(const TopicRaw&);

    static void
    clearSubscriptions();

    /// List of all raw topics currently active.
    static TopicRaw* listOfAllTopics;

    /// Mutex used to protect the publish() method.
    rtos::Mutex mMutex;

    /// Pointer to the list of mSubscriptions
    SubscriptionRaw* mSubscriptions;
};

}  // namespace smpc
}  // namespace outpost

#endif
