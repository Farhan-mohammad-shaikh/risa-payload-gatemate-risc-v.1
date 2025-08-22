/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/fixed_size_array.h>

#include <unittest/harness.h>

using outpost::FixedSizeArray;
using outpost::FixedSizeArrayView;

// ----------------------------------------------------------------------------
class FixedSizeArrayTest : public testing::Test
{
};

TEST_F(FixedSizeArrayTest, createFromCStyleArray)
{
    uint8_t data[7];

    FixedSizeArray<uint8_t, 7> array(data);

    EXPECT_EQ(7U, array.getNumberOfElements());
}

TEST_F(FixedSizeArrayTest, dataAccessWithCStyleArray)
{
    uint8_t data[6] = {6, 5, 4, 3, 2, 1};

    FixedSizeArray<uint8_t, 6> array(data);

    EXPECT_EQ(6U, array.getNumberOfElements());
    EXPECT_EQ(6, array[0]);
    EXPECT_EQ(5, array[1]);
    EXPECT_EQ(2, array[4]);
}

TEST_F(FixedSizeArrayTest, createWithExplicitSize)
{
    uint8_t data[6];

    FixedSizeArray<uint8_t, 4> array = FixedSizeArray<uint8_t, 4>::fromArray(data);

    EXPECT_EQ(4U, array.getNumberOfElements());
}

TEST_F(FixedSizeArrayTest, createArrayOfPointers)
{
    class TestClass
    {
    };

    TestClass testClass1;
    TestClass testClass2;
    TestClass testClass3;

    TestClass* data[3] = {&testClass1, &testClass2, &testClass3};

    FixedSizeArray<TestClass*, 3> array(data);

    EXPECT_EQ(3U, array.getNumberOfElements());

    EXPECT_EQ(&testClass1, array[0]);
    EXPECT_EQ(&testClass2, array[1]);
}

// ----------------------------------------------------------------------------
class FixedSizeArrayViewTest : public testing::Test
{
};

TEST_F(FixedSizeArrayViewTest, createFromCStyleArray)
{
    uint8_t data[7];

    FixedSizeArrayView<uint8_t, 7> array(data);

    EXPECT_EQ(7U, array.getNumberOfElements());
}

TEST_F(FixedSizeArrayViewTest, dataAccessWithCStyleArray)
{
    uint8_t data[6] = {6, 5, 4, 3, 2, 1};

    FixedSizeArrayView<uint8_t, 6> array(data);

    EXPECT_EQ(6U, array.getNumberOfElements());
    EXPECT_EQ(6, array[0]);
    EXPECT_EQ(5, array[1]);
    EXPECT_EQ(2, array[4]);
}

TEST_F(FixedSizeArrayViewTest, createWithExplicitSizeAndOffset)
{
    uint8_t data[6] = {1, 2, 3, 4, 5, 6};

    FixedSizeArrayView<uint8_t, 4> array = FixedSizeArrayView<uint8_t, 4>(data, 2);

    EXPECT_EQ(4U, array.getNumberOfElements());

    EXPECT_EQ(3U, array[0]);
    EXPECT_EQ(4U, array[1]);
}

TEST_F(FixedSizeArrayViewTest, createArrayOfPointers)
{
    class TestClass
    {
    };

    TestClass testClass1;
    TestClass testClass2;
    TestClass testClass3;

    TestClass* data[3] = {&testClass1, &testClass2, &testClass3};

    FixedSizeArrayView<TestClass*, 3> array(data);

    EXPECT_EQ(3U, array.getNumberOfElements());

    EXPECT_EQ(&testClass1, array[0]);
    EXPECT_EQ(&testClass2, array[1]);
}
