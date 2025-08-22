/*
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2024, Pascal Pieper
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_SIP_CONSTANTS_SIP_H_
#define OUTPOST_SIP_CONSTANTS_SIP_H_

#include <limits>
#include <stddef.h>
#include <stdint.h>

namespace outpost
{
namespace sip
{
namespace constants
{
//
// refer to Avionics Systems AVS-SW-TN-0011 Simple Interface Protocol for details.
//
// basically the packet structure is:
//   header (5) + payload (var) + CRC (2)
// in detail:
//   length (2) + unitId (1) + counter (1) + type (1) + payload (var) + crc (2)
static constexpr uint8_t packetHeaderLength = 5;
// the packet overhead (Header + CRC) may also be called packet structure here.
static constexpr uint8_t packetOverhead = 7;
// From the documentation:
// "Length is the number of bytes after this field including CRC. Range from 5 to 65535."
static constexpr size_t maxLengthFieldValueProtocolLimit = std::numeric_limits<uint16_t>::max();

static constexpr uint8_t minPacketSize = packetOverhead;

// Only Parts of the packet structure are counted in the length field.
// The fields following the length are:
//   unitId (1) + counter (1) + type(1) + crc(2)
// This means that the package is 2 bytes larger than the length fields value.
static constexpr uint8_t structureInLength = 5;
static constexpr size_t maxPayloadLengthProtocolLimit =
        maxLengthFieldValueProtocolLimit - structureInLength;

}  // namespace constants
}  // namespace sip
}  // namespace outpost

#endif /* OUTPOST_SIP_PARAMETER_SIP_H_ */
