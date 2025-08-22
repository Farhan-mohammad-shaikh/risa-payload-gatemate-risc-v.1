/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "topic.h"

#include "subscription.h"

#include <outpost/rtos/mutex_guard.h>

outpost::smpc::TopicBase* outpost::smpc::TopicBase::listOfAllTopics = nullptr;

outpost::smpc::TopicBase::TopicBase() :
    ImplicitList<TopicBase>(listOfAllTopics, this), mSubscriptions(nullptr)
{
}

outpost::smpc::TopicBase::~TopicBase()
{
    removeFromList(&TopicBase::listOfAllTopics, this);
}

void
outpost::smpc::TopicBase::publishTypeUnsafe(void* message) const
{
    rtos::MutexGuard lock(mMutex);

    for (Subscription* subscription = mSubscriptions; subscription != nullptr;
         subscription = subscription->mNextTopicSubscription)
    {
        subscription->execute(message);
    }
}

void
outpost::smpc::TopicBase::clearSubscriptions()
{
    for (TopicBase* it = listOfAllTopics; it != nullptr; it = it->getNext())
    {
        it->mSubscriptions = nullptr;
    }
}
