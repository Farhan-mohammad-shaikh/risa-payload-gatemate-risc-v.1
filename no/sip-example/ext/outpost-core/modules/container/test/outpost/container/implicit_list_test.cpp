/*
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Pfeffer, Tobias
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

#include <outpost/container/implicit_list.h>

#include <unittest/harness.h>

#include <inttypes.h>
#include <stdio.h>

using namespace outpost;

class IntElement;

struct IntList
{
    friend class IntElement;

    IntElement* anchor = nullptr;
};

class IntElement : public outpost::ImplicitList<IntElement>
{
public:
    int value;

    explicit IntElement(IntList& list) : ImplicitList(list.anchor, this), value(0)
    {
    }
};

TEST(ImplicitListTest, getNext)
{
    IntList list1;

    IntElement e1(list1);
    IntElement e2(list1);
    IntElement e3(list1);

    EXPECT_EQ(&e2, e3.getNext());
    EXPECT_EQ(&e1, e2.getNext());
    EXPECT_EQ(nullptr, e1.getNext());
}

TEST(ImplicitListTest, removeFromList)
{
    IntList list1;

    IntElement e1(list1);
    IntElement e2(list1);
    IntElement e3(list1);

    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e2);

    EXPECT_EQ(&e1, e3.getNext());

    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e1);

    IntElement e4(list1);
    EXPECT_EQ(&e3, e4.getNext());

    IntElement e5(list1);
    IntElement e6(list1);

    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e6);
    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e3);

    EXPECT_EQ(nullptr, e4.getNext());
    EXPECT_EQ(&e4, e5.getNext());

    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e4);
    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, &e5);

    // removing from empty list is ok
    outpost::ImplicitList<IntElement>::removeFromList(&list1.anchor, nullptr);
}
