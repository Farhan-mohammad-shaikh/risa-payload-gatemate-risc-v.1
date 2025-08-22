/*
 * Copyright (c) 2021, toth_no
 * Copyright (c) 2022-2023, Tobias Pfeffer
 * Copyright (c) 2023-2024, Felix Passenberg
 * Copyright (c) 2023, Kirstein, Hannah
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

#ifndef OUTPOST_SIP_PARAMETER_SIP_H_
#define OUTPOST_SIP_PARAMETER_SIP_H_

#include <outpost/sip/constants.h>

#include <stddef.h>
#include <stdint.h>

#include <type_traits>

namespace outpost
{
namespace sip
{
namespace parameter
{
static constexpr uint16_t maxPayloadLength = 0;

static constexpr size_t maxPacketLength =
        static_cast<size_t>(maxPayloadLength) + constants::packetOverhead;
static_assert(constants::maxPayloadLengthProtocolLimit >= maxPayloadLength,
              "The 2-Byte length field counts the Payload with remaining Header and CRC."
              "The current length does not fit in 2 Bytes."
              "Adjust maxPayloadLength accordingly.");

enum class defaultRequestType : uint8_t
{
    linkTest = 0x01,
    hkRequest = 0x02
    /* 0x02-0x0f reserved */
    /* ======================================= */
    /* 0x10-0x7f application specific requests */
};

enum class defaultResponseType : uint8_t
{
    framingErorr = 0x80,
    errorAck = 0x81,
    successAck = 0x82,
    hkResponse = 0x83
    /* 0x84-0x8f reserved */
    /* ======================================= */
    /* 0x90-0xff application specific response */
};

}  // namespace parameter
}  // namespace sip
}  // namespace outpost

#endif /* OUTPOST_SIP_PARAMETER_SIP_H_ */
