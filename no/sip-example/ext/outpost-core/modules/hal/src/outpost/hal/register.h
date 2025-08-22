/*
 * Copyright (c) 2016-2017, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_HAL_REGISTER_H
#define OUTPOST_HAL_REGISTER_H

#include <outpost/storage/bit_access.h>

#include <stdint.h>

namespace outpost
{
namespace hal
{
/**
 * Register access.
 *
 * Provides methods to read and write complete registers as well as bit fields
 * inside a 32 bit register.
 *
 * \author  Fabian Greif
 */
class Register
{
public:
    template <typename RegisterType, int offsetStart, int offsetEnd, uint32_t registerAddress>
    struct Bitfield
    {
        typedef RegisterType Type;
        static const int start = offsetStart;
        static const int end = offsetEnd;
        static const uint32_t address = registerAddress;
    };

    template <typename RegisterType, int offset, uint32_t registerAddress>
    struct SingleBit
    {
        typedef RegisterType Type;
        static const int start = offset;
        static const int end = offset;
        static const uint32_t address = registerAddress;
    };

    /**
     * Access a register.
     *
     * Allows for 8, 16 or 32 bit access. For example:
     * \code
     * uint16_t value = Register::access<uint16_t>(0x00000000);
     * \endcode
     *
     * \tparam T
     *      Register type. Valid types are uint8_t, uint16_t and uint32_t.
     * \param address
     *      Address of the register.
     * \return
     *      Access object for the register.
     */
    template <typename T>
    static inline volatile T&
    access(uint32_t address);

    template <typename T>
    static inline T*
    getPointer(uint32_t address);

    template <typename T>
    static inline uint32_t
    read();

    template <typename T>
    static inline uint32_t
    readWithOffset(size_t offset);

    template <typename T>
    static inline uint32_t
    readFromMemory(uint32_t& memory);

    template <typename T>
    static inline void
    write(uint32_t value);

    /**
     * Write the value directly into the register without reading it first.
     *
     * All other bits apart from the one provided through \c value are set
     * to zero.
     *
     * \warning This might overwrite other values already stored in
     *          the register!
     */
    template <typename T>
    static inline void
    overwrite(uint32_t value);

    template <typename T>
    static inline void
    writeWithOffset(uint32_t value, size_t offset);

    template <typename T>
    static inline void
    writeToMemory(uint32_t value, uint32_t& memory);

    template <typename T>
    static inline uint32_t
    getValue(uint32_t value);

    template <typename T>
    static inline uint32_t
    getMask();

private:
    // Disable destructor, copy-constructor and copy assignment operator
    Register();

    ~Register();

    Register(const Register&);

    Register&
    operator=(const Register&);
};

}  // namespace hal
}  // namespace outpost

#include "register_impl.h"

#endif
