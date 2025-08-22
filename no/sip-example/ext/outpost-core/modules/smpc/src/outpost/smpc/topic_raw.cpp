/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "topic_raw.h"

#include "subscription_raw.h"

#include <outpost/rtos/mutex_guard.h>

outpost::smpc::TopicRaw* outpost::smpc::TopicRaw::listOfAllTopics = 0;

outpost::smpc::TopicRaw::TopicRaw() :
    ImplicitList<TopicRaw>(listOfAllTopics, this), mSubscriptions(0)
{
}

outpost::smpc::TopicRaw::~TopicRaw()
{
    removeFromList(&TopicRaw::listOfAllTopics, this);
}

void
outpost::smpc::TopicRaw::publish(const void* message, size_t length)
{
    rtos::MutexGuard lock(mMutex);

    for (SubscriptionRaw* subscription = mSubscriptions; subscription != 0;
         subscription = subscription->mNextTopicSubscription)
    {
        subscription->execute(message, length);
    }
}

void
outpost::smpc::TopicRaw::clearSubscriptions()
{
    for (TopicRaw* it = listOfAllTopics; it != 0; it = it->getNext())
    {
        it->mSubscriptions = 0;
    }
}
