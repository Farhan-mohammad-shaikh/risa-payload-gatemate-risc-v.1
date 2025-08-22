/*
 * Copyright (c) 2017-2019, Fabian Greif
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_SERIALIZE_TRAITS_H
#define OUTPOST_UTILS_SERIALIZE_TRAITS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <type_traits>

namespace outpost
{
template <typename T, typename = void>
struct SerializeBigEndianTraits
{
    static void
    store(uint8_t*& buffer, T value);

    static T
    read(const uint8_t*& buffer);

    static T
    peek(const uint8_t* const buffer, size_t offset);

    static constexpr size_t
    size();
};

template <>
struct SerializeBigEndianTraits<bool>
{
    static inline void
    store(uint8_t*& buffer, bool data)
    {
        buffer[0] = data ? 1U : 0U;
        buffer += 1;
    }

    static inline bool
    read(const uint8_t*& buffer)
    {
        uint8_t value;
        value = buffer[0];
        buffer += 1;

        return value != 0;
    }

    static inline bool
    peek(const uint8_t* const buffer, size_t n)
    {
        return buffer[n] != 0;
    }

    static inline constexpr size_t
    size()
    {
        return 1;
    }
};

template <typename T>
struct SerializeBigEndianTraits<
        T,
        typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type>
{
    static constexpr size_t numberOfBytes = sizeof(T);

    static inline void
    store(uint8_t*& buffer, T data)
    {
        for (size_t index = 0; index < numberOfBytes; index++)
        {
            buffer[index] = static_cast<uint8_t>(data >> (numberOfBytes - index - 1) * 8);
        }
        buffer += numberOfBytes;
    }

    static inline T
    read(const uint8_t*& buffer)
    {
        T value = 0;
        for (size_t index = 0; index < numberOfBytes; index++)
        {
            value |= static_cast<T>(buffer[index]) << (numberOfBytes - index - 1) * 8;
        }
        buffer += numberOfBytes;
        return value;
    }

    static inline T
    peek(const uint8_t* const buffer, size_t n)
    {
        T value = 0;
        for (size_t index = 0; index < numberOfBytes; index++)
        {
            value |= static_cast<T>(buffer[index + n]) << (numberOfBytes - index - 1) * 8;
        }
        return value;
    }

    static inline constexpr size_t
    size()
    {
        return numberOfBytes;
    }
};

template <>
struct SerializeBigEndianTraits<float>
{
    static inline void
    store(uint8_t*& buffer, float data)
    {
        // cppcheck-suppress invalidPointerCast
        const uint32_t* ptr = reinterpret_cast<const uint32_t*>(&data);
        SerializeBigEndianTraits<uint32_t>::store(buffer, *ptr);
    }

    static inline float
    read(const uint8_t*& buffer)
    {
        float f;
        const uint32_t value = SerializeBigEndianTraits<uint32_t>::read(buffer);

        memcpy(&f, &value, sizeof(f));
        return f;
    }

    static inline float
    peek(const uint8_t* const buffer, size_t n)
    {
        float f;
        const uint32_t value = SerializeBigEndianTraits<uint32_t>::peek(buffer, n);

        memcpy(&f, &value, sizeof(f));
        return f;
    }

    static inline constexpr size_t
    size()
    {
        return 4;
    }
};

template <>
struct SerializeBigEndianTraits<double>
{
    static inline void
    store(uint8_t*& buffer, double data)
    {
        // cppcheck-suppress invalidPointerCast
        const uint64_t* ptr = reinterpret_cast<const uint64_t*>(&data);
        SerializeBigEndianTraits<uint64_t>::store(buffer, *ptr);
    }

    static inline double
    read(const uint8_t*& buffer)
    {
        double d;
        const uint64_t value = SerializeBigEndianTraits<uint64_t>::read(buffer);

        memcpy(&d, &value, sizeof(d));
        return d;
    }

    static inline double
    peek(const uint8_t* const buffer, size_t n)
    {
        double d;
        const uint64_t value = SerializeBigEndianTraits<uint64_t>::peek(buffer, n);

        memcpy(&d, &value, sizeof(d));
        return d;
    }

    static inline constexpr size_t
    size()
    {
        return 8;
    }
};

}  // namespace outpost

#endif
