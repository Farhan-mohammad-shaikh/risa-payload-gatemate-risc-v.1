/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Pfeffer, Tobias
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/reference_queue.h>

#include <unittest/harness.h>

#include <stdint.h>

using namespace outpost::container;
using namespace outpost::utils;
using namespace outpost::time;

TEST(reference_queue, status)
{
    ReferenceQueue<int, 3> refq1;

    EXPECT_TRUE(refq1.isEmpty());
    EXPECT_FALSE(refq1.isFull());

    int a = 1;
    int b = 2;
    int c = 3;

    EXPECT_EQ(refq1.send(a), OperationResult::success);
    EXPECT_EQ(refq1.send(b), OperationResult::success);
    EXPECT_EQ(refq1.send(c), OperationResult::success);

    EXPECT_FALSE(refq1.isEmpty());
    EXPECT_TRUE(refq1.isFull());
}

TEST(reference_queue, sendingAndReceiving)
{
    ReferenceQueue<int, 3> refq1;

    EXPECT_TRUE(refq1.isEmpty());

    int sData1 = 1;
    int rData1;

    EXPECT_EQ(refq1.send(sData1), OperationResult::success);

    EXPECT_EQ(1, refq1.getNumberOfItems());
    auto res = refq1.receive(Seconds(1));
    ASSERT_TRUE(res);
    rData1 = *res;

    EXPECT_EQ(1, rData1);

    int sData2 = 2;
    int rData2;
    int sData3 = 3;
    int rData3;
    int sData4 = 4;
    int rData4;

    EXPECT_EQ(refq1.send(sData2), OperationResult::success);
    // 2
    EXPECT_EQ(1, refq1.getNumberOfItems());

    EXPECT_EQ(refq1.send(sData3), OperationResult::success);
    // 2,3
    EXPECT_EQ(2, refq1.getNumberOfItems());

    res = refq1.receive();
    ASSERT_TRUE(res);
    rData2 = *res;
    EXPECT_EQ(2, rData2);
    // 3
    EXPECT_EQ(1, refq1.getNumberOfItems());

    EXPECT_EQ(refq1.send(sData4), OperationResult::success);
    // 3,4
    EXPECT_EQ(2, refq1.getNumberOfItems());
    res = refq1.receive();
    ASSERT_TRUE(res);  //==3
    rData3 = *res;
    EXPECT_EQ(3, rData3);
    // 4
    EXPECT_EQ(1, refq1.getNumberOfItems());
    res = refq1.receive();
    ASSERT_TRUE(res);
    rData4 = *res;
    EXPECT_EQ(4, rData4);
    EXPECT_EQ(0, refq1.getNumberOfItems());
}

TEST(reference_queue, errors)
{
    ReferenceQueue<int, 3> refq1;

    int a = 1;
    int b = 2;
    int c = 3;

    auto res = refq1.receive(Milliseconds(200));
    ASSERT_FALSE(res);

    EXPECT_EQ(refq1.send(a, Milliseconds(200)), OperationResult::success);
    EXPECT_EQ(refq1.send(b, Milliseconds(200)), OperationResult::success);
    EXPECT_EQ(refq1.send(c, Milliseconds(200)), OperationResult::success);
    EXPECT_EQ(refq1.send(a, Milliseconds(200)), OperationResult::genericError);

    EXPECT_FALSE(refq1.isEmpty());
    EXPECT_TRUE(refq1.isFull());
}
