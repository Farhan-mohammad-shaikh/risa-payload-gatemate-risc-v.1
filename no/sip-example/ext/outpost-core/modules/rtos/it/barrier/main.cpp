/*
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

#include <outpost/rtos.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <array>

namespace testing
{
#include "../../arch/freertos/outpost/rtos/barrier.cpp"
}

class TesterThread : public outpost::rtos::Thread
{
public:
    TesterThread() : outpost::rtos::Thread(100), mNumber(0){};

    testing::outpost::rtos::Barrier* mBarrier;
    uint32_t mNumber;

    void
    run()
    {
        mBarrier->wait();
        mNumber++;
        mBarrier->wait();
        mNumber++;
        mBarrier->wait();
        mNumber++;
        mBarrier->wait();
        mNumber++;
        mBarrier->wait();
        mNumber++;
        mBarrier->wait();
        while (true)
        {
            outpost::rtos::Thread::sleep(outpost::time::Seconds(1));
        }
    }
};

int
main(void)
{
    constexpr uint32_t threadcount = 100;
    std::array<TesterThread, threadcount> threads;

    // yourself also needed
    testing::outpost::rtos::Barrier barrier(threadcount + 1);

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        it->mBarrier = &barrier;
        it->start();
    }

    // give all time to start
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 0)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("First wait done\n");

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 1)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("Second wait done\n");

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 2)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("Third wait done\n");

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 3)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("Forth wait done\n");

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 4)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("Fifth wait done\n");

    for (auto it = threads.begin(); it != threads.end(); ++it)
    {
        if (it->mNumber != 5)
        {
            printf("Thread skipped Barrier value = %d\n", it->mNumber);
        }
    }
    barrier.wait();

    // give all time to count
    outpost::rtos::Thread::sleep(outpost::time::Seconds(1));

    printf("Done\n");

    // outpost Threads cannot end
    exit(0);

    // prevents warning
    return 0;
}
