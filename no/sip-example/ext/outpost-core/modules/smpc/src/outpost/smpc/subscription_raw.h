/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SMPC_SUBSCRIPTION_RAW_H
#define OUTPOST_SMPC_SUBSCRIPTION_RAW_H

#include "subscriber.h"
#include "topic_raw.h"

#include <outpost/utils/functor.h>

#include <stddef.h>

namespace outpost
{
namespace smpc
{
/**
 * Subscription to a topic.
 *
 * Every component that wants to receive data from a topic needs to
 * Instantiate one or more objects of this class. Each instance binds
 * a topic to a member function of the subscribing class. Data send to
 * the topic will be forwarded to the given member function.
 *
 * To avoid the overhead of generating a copy of this class for every
 * type of topic and subscriber this class works internally with void
 * pointers.
 *
 * \ingroup smpc
 * \author  Fabian Greif
 */
class SubscriptionRaw : public ImplicitList<SubscriptionRaw>,
                        protected Functor<void(const void* message, size_t length)>
{
public:
    friend class TopicRaw;

    /**
     * Constructor.
     *
     * Binds a subscriber to a topic.
     *
     * \param[in]    topic
     *         Raw Topic to subscribe to
     * \param[in]    subscriber
     *         Subscribing class. Must be a subclass of outpost::com::Subscriber.
     * \param[in]    function
     *         Member function pointer of the subscribing class.
     */
    template <typename S>
    SubscriptionRaw(TopicRaw& topic, S* subscriber, typename FunctionType<S>::Type function);

    /**
     * Destroy the subscription
     *
     * \warning    The destruction and creation of subscriptions during the normal
     *             runtime is not thread-safe. If topics need to be
     *             destroyed outside the initialization of the application
     *             it is necessary to hold all other threads which
     *             might also create or destroy topics and/or subscriptions.
     */
    ~SubscriptionRaw();

    /**
     * Connect all subscriptions to it's assigned topic.
     *
     * Has to be called at program startup to initialize the
     * Publisher<>Subscriber protocol.
     *
     * \internal
     * Builds the internal linked lists.
     */
    static void
    connectSubscriptionsToTopics();

protected:
    /**
     * Release all subscriptions.
     *
     * Counterpart to connectSubscriptionsToTopics(). Use of this
     * function is not thread-safe. To use halt all threads that
     * might create or destroy subscriptions or topics.
     */
    static void
    releaseAllSubscriptions();

private:
    // Disable default constructor
    SubscriptionRaw();

    // Disable copy constructor
    SubscriptionRaw(const SubscriptionRaw&);

    // Disable copy assignment operator
    SubscriptionRaw&
    operator=(const SubscriptionRaw&);

    // List of all subscriptions currently in the system
    static SubscriptionRaw* listOfAllSubscriptions;

    // Used by Subscription::connect to map the subscriptions to
    // their corresponding topics.
    TopicRaw* const mTopic;
    SubscriptionRaw* mNextTopicSubscription;
};

// ----------------------------------------------------------------------------
// Implementation of the template constructor
template <typename S>
SubscriptionRaw::SubscriptionRaw(TopicRaw& topic,
                                 // False positive: subscriber cannot be pointer to const.
                                 // cppcheck-suppress constParameterPointer
                                 S* subscriber,
                                 typename FunctionType<S>::Type function) :
    ImplicitList<SubscriptionRaw>(listOfAllSubscriptions, this),
    Functor<void(const void* message, size_t length)>(*subscriber, function),
    mTopic(&topic),
    mNextTopicSubscription(0)
{
}

}  // namespace smpc
}  // namespace outpost

#endif
