/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_FIXED_ORDERED_MAP_IMPL_H
#define OUTPOST_UTILS_FIXED_ORDERED_MAP_IMPL_H

#include "fixed_ordered_map.h"

template <typename Entry, typename Key>
outpost::FixedOrderedMap<Entry, Key>::FixedOrderedMap(Entry* entries, size_t numberOfEntries) :
    mEntries(entries), mNumberOfEntries(numberOfEntries)
{
}

template <typename Entry, typename Key>
const Entry*
outpost::FixedOrderedMap<Entry, Key>::getEntry(Key key) const
{
    int imax = static_cast<int>(mNumberOfEntries) - 1;
    int imin = 0;

    Entry* entry = 0;
    while ((imax >= imin) && (entry == 0))
    {
        int mid = imin + ((imax - imin) / 2);

        if (mEntries[mid].mKey == key)
        {
            entry = &mEntries[mid];
        }
        else if (mEntries[mid].mKey < key)
        {
            imin = mid + 1;
        }
        else
        {
            imax = mid - 1;
        }
    }

    return entry;
}

#endif
