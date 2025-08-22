/*
 * Copyright (c) 2020-2022, Jan-Gerd Mess
 * Copyright (c) 2021, Jan Malburg
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

#ifndef OUTPOST_SWB_BUS_SUBSCRIPTION_H
#define OUTPOST_SWB_BUS_SUBSCRIPTION_H

#include "types.h"

#include <outpost/container/list.h>

#include <stdint.h>

namespace outpost
{
namespace swb
{
/**
 * A BusSubscription can be used together with a SubscriptionFilter on a BusChannel to filter by
 * (masked) Message id.
 * \ingroup swb
 * \param IDType type of the message ID
 */
template <typename IDType>
class BusSubscription : public outpost::ListElement
{
public:
    /**
     * Constructor.
     * \param messageId Incoming Message are filtered based on this ID.
     * \param mask A bitmask can be set to ignore certain bits of the ID during matching.
     *             Example: A mask of all zeros lets all Message pass, whereas a mask of all ones
     *             (default) means exact matching of the given messageId.
     */
    explicit BusSubscription(IDType messageId, IDType mask = ~(IDType())) :
        mMessageId(messageId & mask), mMask(mask), mNumMatchedMessages(0U)
    {
    }

    /**
     * Destructor.
     */
    virtual ~BusSubscription() = default;

    /**
     * Matching function for a given ID
     * \return Returns true if the given id matches the subscription, false otherwise.
     */
    inline bool
    matches(IDType id) const
    {
        bool res = false;
        if ((id & mMask) == mMessageId)
        {
            res = true;
            mNumMatchedMessages++;
        }
        return res;
    }

    /**
     * Getter for the number of matched Message
     * \return Number of matched Message.
     */
    inline uint32_t
    getNumberOfMatchedMessages() const
    {
        return mNumMatchedMessages;
    }

private:
    IDType mMessageId;
    IDType mMask;

    mutable uint32_t mNumMatchedMessages;
};

}  // namespace swb
}  // namespace outpost

#endif /* OUTPOST_SWB_BUS_SUBSCRIPTION_H */
