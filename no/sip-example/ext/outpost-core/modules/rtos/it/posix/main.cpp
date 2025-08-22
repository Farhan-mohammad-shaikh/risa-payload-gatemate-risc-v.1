/*
 * Copyright (c) 2014-2017, 2019, Fabian Greif
 * Copyright (c) 2021, Felix Passenberg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <stdint.h>
#include <stdio.h>

// POSIX specific
#include "../reference/consumer.h"
#include "../reference/producer.h"

#include <outpost/rtos/internal/time.h>

outpost::rtos::Queue<uint32_t> queue(10);

Producer producer(queue);
Consumer consumer(queue);

int
main(void)
{
    timespec relative =
            outpost::rtos::toRelativeTime(std::numeric_limits<outpost::time::Duration>::max());
    printf("%lu %li\n", relative.tv_sec, relative.tv_nsec);

    timespec negative = outpost::rtos::toRelativeTime(-outpost::time::Milliseconds(1500));
    printf("%li %li\n", negative.tv_sec, negative.tv_nsec);

    timespec absolute = outpost::rtos::toAbsoluteTime(
            CLOCK_MONOTONIC, std::numeric_limits<outpost::time::Duration>::max());
    printf("%lu %li\n", absolute.tv_sec, absolute.tv_nsec);

    producer.start();
    consumer.start();

    while (1)
    {
        consumer.waitForNewValue();
        uint32_t value = consumer.getCurrentValue();

        printf("value: %i\n", static_cast<int>(value));
    }
}
