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

#ifndef OUTPOST_CRC_GENERIC_H
#define OUTPOST_CRC_GENERIC_H

#include "crc_table.h"

#include <outpost/base/slice.h>

#include <stddef.h>
#include <stdint.h>

#include <cstdint>

namespace outpost
{

template <typename T, T polynomial, T initialValue = 0, bool reversed = false, T finalXor = 0x0>
class CrcGeneric
{
public:
    inline CrcGeneric() : mCrc(initialValue)
    {
    }

    inline ~CrcGeneric()
    {
    }

    /**
     * Calculate CRC from a block of data.
     *
     * \param data
     *     address
     * \param length
     *     length
     *
     * \retval crc
     *     calculated checksum
     */
    static T
    calculate(outpost::Slice<const uint8_t> data)
    {
        CrcGeneric<T, polynomial, initialValue, reversed, finalXor> generator;
        for (size_t i = 0; i < data.getNumberOfElements(); ++i)
        {
            generator.update(data[i]);
        }

        T value = generator.getValue();

        return value;
    }

    /**
     * Reset CRC calculation
     */
    inline void
    reset()
    {
        mCrc = initialValue;
    }

    /**
     * CRC update.
     *
     * \param data
     *     byte
     */
    void
    update(uint8_t data)
    {
        // Any decent compiler should optimize this condition
        if (reversed)
        {
            mCrc = (mCrc >> numberOfBitsPerByte) ^ crcTable[(mCrc ^ data) & 0xFF];
        }
        else
        {
            const uint8_t rightShift = (sizeof(T) - 1) * numberOfBitsPerByte;
            mCrc = (mCrc << numberOfBitsPerByte) ^ crcTable[((mCrc >> rightShift) ^ data) & 0xFF];
        }
    }

    /**
     * Get result of CRC calculation.
     */
    inline T
    getValue() const
    {
        return (finalXor ^ mCrc);
    }

private:
    //  disable copy constructor
    CrcGeneric(const CrcGeneric&);

    // disable copy-assignment operator
    CrcGeneric&
    operator=(const CrcGeneric&);

    static constexpr int32_t numberOfValuesPerByte = 256;
    static constexpr uint8_t numberOfBitsPerByte = 8;

    /// Pre-calculated CRC table for one byte
    static constexpr CrcTable<T, polynomial, reversed> crcTable =
            CrcTable<T, polynomial, reversed>{};

    T mCrc;
};

template <typename T, T polynomial, T initialValue, bool reversed, T finalXor>
constexpr CrcTable<T, polynomial, reversed>
        CrcGeneric<T, polynomial, initialValue, reversed, finalXor>::crcTable;

}  // namespace outpost

#endif
