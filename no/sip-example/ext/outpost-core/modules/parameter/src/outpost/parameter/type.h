/*
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_PARAMETER_TYPE_H_
#define OUTPOST_PARAMETER_TYPE_H_

#include <outpost/time.h>

#include <stdint.h>

#include <type_traits>

namespace outpost
{
namespace parameter
{
class Type;

template <typename T>
class Marker
{
    friend class Type;

private:
    static int mark;
};

template <typename T>
int Marker<T>::mark;

class Type
{
public:
    /**
     * Get the type by setting the template parameter
     *
     * @param T  Template parameter to choose the type
     * @return the Type for T
     */
    template <typename T>
    static Type
    getType()
    {
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
        Type t;
        t.markPointer = &Marker<type>::mark;
        return t;
    }

    /**
     * Get the type from a variable
     *
     * @param T  Template parameter to choose the type
     * @return the Type for T
     */
    template <typename T>
    static Type
    getType(const T& /*t*/)
    {
        typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
        Type t;
        t.markPointer = &Marker<type>::mark;
        return t;
    }

    bool
    operator==(const Type& o) const
    {
        return markPointer == o.markPointer;
    }

    bool
    operator!=(const Type& o) const
    {
        return markPointer != o.markPointer;
    }

private:
    Type() = default;
    int* markPointer = nullptr;
};

}  // namespace parameter
}  // namespace outpost

#endif
