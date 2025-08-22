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

#ifndef OUTPOST_CRC16_H
#define OUTPOST_CRC16_H

#include "crc_generic.h"

namespace outpost
{

/**
 * CRC-16 calculation
 * Without specifying an alternative polynomial or initial
 * value CRC-16-CCITT calculation with the following
 * polynomial and initial value is performed.
 *
 * Polynomial    : x^16 + x^12 + x^5 + 1 (0x1021, MSB first)
 * Initial value : 0xFFFF
 *
 * Used for space packet transfer frames.
 *
 * \ingroup crc
 * \author  Fabian Greif
 */
template <uint16_t polynomial = 0x1021, uint16_t initialValue = 0xFFFF>
using Crc16 = CrcGeneric<uint16_t, polynomial, initialValue>;

using Crc16Ccitt = Crc16<0x1021, 0xFFFF>;

}  // namespace outpost

#endif
