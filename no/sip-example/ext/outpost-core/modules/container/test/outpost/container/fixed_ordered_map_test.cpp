/*
 * Copyright (c) 2015-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/fixed_ordered_map.h>

#include <unittest/harness.h>

using namespace outpost;

struct Entry
{
    uint16_t mKey;
    uint32_t mValue;
};

TEST(FixedOrderedMapTest, createList)
{
    Entry entries[] = {
            Entry({1235, 0}),
            Entry({1236, 10}),
    };

    FixedOrderedMap<Entry, uint16_t> list(entries, 2);

    EXPECT_EQ(2U, list.getNumberOfElements());
}

TEST(FixedOrderedMapTest, createMapFromArrayWithoutExplicitSize)
{
    Entry entries[] = {
            Entry({1, 0}),
            Entry({3, 1}),
            Entry({5, 2}),
            Entry({17, 3}),
    };

    FixedOrderedMap<Entry, uint16_t> list(entries);

    EXPECT_EQ(4U, list.getNumberOfElements());
}

TEST(FixedOrderedMapTest, availableEntriesShouldBeReturnedByPointer)
{
    Entry entries[] = {
            Entry({1, 0}),
            Entry({3, 1}),
            Entry({5, 2}),
            Entry({17, 3}),
    };

    FixedOrderedMap<Entry, uint16_t> list(entries);

    EXPECT_EQ(0U, list.getEntry(1)->mValue);
    EXPECT_EQ(1U, list.getEntry(3)->mValue);
    EXPECT_EQ(2U, list.getEntry(5)->mValue);
    EXPECT_EQ(3U, list.getEntry(17)->mValue);
}

TEST(FixedOrderedMapTest, missingEntriesShouldReturnANullPointer)
{
    Entry entries[] = {
            Entry({1, 0}),
            Entry({3, 1}),
            Entry({5, 2}),
            Entry({17, 3}),
    };

    FixedOrderedMap<Entry, uint16_t> list(entries);

    EXPECT_EQ(0, list.getEntry(0));
    EXPECT_EQ(0, list.getEntry(2));
    EXPECT_EQ(0, list.getEntry(4));
    EXPECT_EQ(0, list.getEntry(35000));
}
