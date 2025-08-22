/*
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "harness.h"

::testing::AssertionResult
StringsMatch(const char* a, const char* b)
{
    if (strlen(a) != strlen(b))
        return ::testing::AssertionFailure()
               << "Size differs, " << strlen(a) << " != " << strlen(b);

    for (size_t i(0); i < strlen(a); i++)
    {
        if (a[i] != b[i])
        {
            return ::testing::AssertionFailure() << "array[" << i << "] (" << a[i]
                                                 << ") != expected[" << i << "] (" << b[i] << ")";
        }
    }
    return ::testing::AssertionSuccess();
}
