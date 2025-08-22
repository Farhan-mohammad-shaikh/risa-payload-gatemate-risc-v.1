/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2017, Jan Sommer
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "bounded_string_helper.h"

std::string
to_string(const outpost::BoundedString& from)
{
    return std::string(from.begin(), from.end());
}

template <>
std::string
testing::PrintToString(const outpost::BoundedString& from)
{
    return to_string(from);
}
