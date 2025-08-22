/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_BIT_ACCESS_H
#define OUTPOST_UTILS_BIT_ACCESS_H

#include <stddef.h>
#include <stdint.h>

namespace outpost
{
/**
 * Bit field access.
 *
 * Provides access to single or multiple bits in a larger variable.
 *
 * Uses LSB 0 bit numbering (bit 0 is the LSB).
 *
 * \author  Fabian Greif
 */
class BitAccess
{
public:
    /**
     * Get a single bit.
     *
     * \tparam T
     *      Register type. Valid types are uint8_t, uint16_t and uint32_t.
     * \tparam offset
     *      Offset of the bit within the register.
     *
     * \param data
     *      Value from which to extract the bit value.
     *
     * \retval true   Bit is set.
     * \retval false  Bit is cleared.
     */
    template <typename T, int offset>
    static bool
    get(const T& data);

    /**
     * Read multiple bits within a value.
     *
     * \tparam T
     *      Register type. Valid types are uint8_t, uint16_t and uint32_t.
     * \tparam end
     *      End of the bitfield to read.
     * \tparam start
     *      Start of the bitfield to read.
     *
     * \param data
     *      Value from which to extract the bitfield value.
     *
     * \return
     *      Value of the bits in the field defined through \c offset and \c width.
     */
    template <typename T, int end, int start>
    static T
    get(const T& data);

    /**
     * Set a single bit.
     *
     * \tparam T
     *      Register type. Valid types are uint8_t, uint16_t and uint32_t.
     * \tparam offset
     *      Offset of the bit within the register.
     *
     * \param address
     *      Address of the register.
     * \param value
     *      Set bit if \p value is true, otherwise clear it.
     */
    template <typename T, int offset>
    static void
    set(T& data, bool value);

    /**
     * Set multiple bits within a register.
     *
     * \tparam T
     *      Register type. Valid types are uint8_t, uint16_t and uint32_t.
     * \tparam end
     *      End of the bitfield to write.
     * \tparam start
     *      Start of the bitfield to write.
     *
     * \param address
     *      Address of the register.
     * \param value
     *      Value to set in the field defined through \c offset and \c width.
     */
    template <typename T, int end, int start>
    static void
    set(T& data, T value);

    template <typename T>
    static T
    getMask(size_t width);

private:
    // Disable compiler generated functions
    BitAccess();

    ~BitAccess();

    BitAccess(const BitAccess& other);

    BitAccess&
    operator=(const BitAccess& other);

    static const int numberOfBitsPerByte = 8;
};

}  // namespace outpost

#include "bit_access_impl.h"

#endif
