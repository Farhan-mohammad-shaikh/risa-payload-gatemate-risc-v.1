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

#ifndef OUTPOST_CRC32_H
#define OUTPOST_CRC32_H

#include "crc_generic.h"

namespace outpost
{

/**
 * CRC-32 calculation
 * Without specifying an alternative polynomial or initial value
 * or final XOR value CRC-32-CCITT calculation with the following
 * polynomial, initial value and final XOR is performed.
 *
 * Polynomial    : X^32 + X^26 + X^23 + X^22 + X^16 + X^12 + X^11 +
 *                 X^10 + X^8 + X^7 + X^5 + X^4 + X^2 + X + 1
 *                 (0xEDB88320, LSB first)
 * Initial value : 0xFFFFFFFF
 * Final XOR     : 0xFFFFFFFF
 *
 * Used in IEEE 802.3 and PNG [2], see also ISO 3309 or ITU-T V.42, [1] and [3]
 *
 * [1] https://users.ece.cmu.edu/~koopman/crc/crc32.html
 * [2] http://www.w3.org/TR/PNG/#D-CRCAppendix
 * [3] http://www.greenend.org.uk/rjk/tech/crc.html
 *
 * \ingroup crc
 * \author  Fabian Greif
 */
template <uint32_t polynomial = 0xEDB88320,
          uint32_t initialValue = 0xFFFFFFFF,
          uint32_t finalXOR = 0xFFFFFFFF>
using Crc32Reversed_ = CrcGeneric<uint32_t, polynomial, initialValue, true, finalXOR>;

using Crc32Reversed = Crc32Reversed_<0xEDB88320, 0xFFFFFFFF, 0xFFFFFFFF>;

}  // namespace outpost

#endif
