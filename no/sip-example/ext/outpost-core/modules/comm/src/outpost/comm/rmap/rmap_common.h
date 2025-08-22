/*
 * Copyright (c) 2017, Muhammad Bassam
 * Copyright (c) 2019-2020, Jan Malburg
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_COMMON_H_
#define OUTPOST_COMM_RMAP_COMMON_H_

#include <stdint.h>

namespace outpost
{
namespace comm
{
namespace rmap
{
static constexpr uint16_t maxTransactionId = UINT16_MAX;
static constexpr uint8_t maxConcurrentTransactions = 10;
static constexpr uint16_t bufferSize = 1024;  // The amount of data bytes.
static constexpr uint8_t numberOfReceiveBuffers =
        3;  // How many reply packages can be queued,
            // including the currently processed one (i.e. min = 1)
static constexpr uint8_t defaultLogicalAddress = 0xFE;
static constexpr uint8_t defaultExtendedAddress = 0x00;
static constexpr uint8_t protocolIdentifier = 0x01;

static constexpr uint16_t writeCommandOverhead = 17;  // including data crc
static constexpr uint16_t readCommandOverhead = 16;
static constexpr uint16_t readReplyOverhead = 13;  // includes data crc
static constexpr uint16_t writeReplyOverhead = 8;

static constexpr uint8_t minimumReplySize = writeReplyOverhead;
// Maximum physical output ports that router can have (see ECSS-E-ST-50-12C pg. 98)
static constexpr uint8_t maxPhysicalRouterOutputPorts = 32;

// Maximum nodes is specified from the number of SpW target addresses that
// can be accommodated into a single RMAP packet (see ECSS-E-ST-50-52C pg. 24)
static const uint8_t maxNumberOfTargetNodes = 12;
static const uint8_t maxNodeNameLength = 20;

}  // namespace rmap
}  // namespace comm
}  // namespace outpost

#endif /* OUTPOST_COMM_RMAP_COMMON_H_ */
