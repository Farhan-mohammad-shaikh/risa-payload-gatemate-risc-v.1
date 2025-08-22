/*
 * Copyright (c) 2013-2018, Fabian Greif
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2023, Peter Ohr
 * Copyright (c) 2023, Jan Sommer
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_CRC8_H
#define OUTPOST_CRC8_H

#include "crc_generic.h"

namespace outpost
{

/**
 * CRC-8 calculation
 * Without specifying an alternative polynomial or initial
 * value CRC-8-CCITT calculation with the following
 * polynomial and initial value is performed.
 *
 * Polynomial    : x^8 + x^2 + x + 1 (0x07, MSB first)
 * Initial value : 0x00
 *
 * \ingroup crc
 * \author  Fabian Greif
 */
template <uint8_t polynomial = 0x07, uint8_t initialValue = 0x00>
using Crc8 = CrcGeneric<uint8_t, polynomial, initialValue, false>;

using Crc8Ccitt = Crc8<0x07, 0x00>;

/**
 * Reversed CRC-8 calculation for RMAP
 * Without specifying an alternative polynomial or initial
 * value CRC-8-CCITT calculation with the following
 * polynomial and initial value is performed.
 *
 * Polynomial    : x^8 + x^2 + x + 1 (0xE0, LSB first)
 * Initial value : 0x00
 *
 * \see     ECSS-E-50-11 SpaceWire RMAP Protocol
 *
 * \ingroup crc
 * \author  Fabian Greif
 */
template <uint8_t polynomial = 0xE0, uint8_t initialValue = 0x00>
using Crc8Reversed = CrcGeneric<uint8_t, polynomial, initialValue, true>;

using Crc8CcittReversed = Crc8Reversed<0xE0, 0x00>;
}  // namespace outpost

#endif
