/*
 * Copyright (c) 2017, Fabian Greif
 * Copyright (c) 2021, pfef_to
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_SERIALIZABLE_OBJECT_H
#define OUTPOST_UTILS_SERIALIZABLE_OBJECT_H

#include "serialize.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

namespace outpost
{
/**
 * Abstract base class for objects than can be serialized.
 *
 * \author  Fabian Greif
 */
class SerializableObject
{
public:
    SerializableObject() = default;

    virtual ~SerializableObject();

    SerializableObject(const SerializableObject&) = default;

    SerializableObject&
    operator=(const SerializableObject&) = default;

    /**
     * Get the size of the parameter.
     *
     * \return  Size of the parameter in bytes.
     */
    virtual size_t
    getSerializedSize() const = 0;

    /**
     * Write parameter value onto the output stream.
     *
     * \param stream
     *      Output stream
     */
    virtual void
    serialize(Serialize& stream) const = 0;

    /**
     * Read parameter value from stream.
     *
     * \param stream
     *      Input stream
     * \retval  true    Parameter could be read and validated.
     * \retval  false   Parameter validation failed.
     */
    virtual bool
    deserialize(Deserialize& stream) = 0;
};

}  // namespace outpost

#endif
