/*
 * Copyright (c) 2013-2017, Fabian Greif
 * Copyright (c) 2022, Tobias Pfeffer
 * Copyright (c) 2023, Jan-Gerd Mess
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_IMPLICIT_LIST_H
#define OUTPOST_UTILS_IMPLICIT_LIST_H

namespace outpost
{
/**
 * Static single linked list.
 *
 * TODO example
 *
 * This implementation relies for the \c next member on the fact that
 * zero-initialized static variables are initialized before any constructor
 * is called.
 *
 * See chapter 3.6.2 of the C++03 standard:
 *
 *     Objects with static storage duration (3.7.1) shall be
 *     zero-initialized (8.5) before any other initialization
 *     takes place. Zero-initialization and initialization with a
 *     constant expression are collectively called static
 *     initialization; all other initialization is dynamic
 *     initialization. Objects of POD types (3.9) with static storage
 *     duration initialized with constant expressions (5.19) shall be
 *     initialized before any dynamic initialization takes place.
 *
 * This does only apply to static member variables of classes and
 * global variables.
 */
template <typename T>
class ImplicitList
{
public:
    /**
     * Add element to the list.
     *
     * For an example on how to use this class see topic.cpp
     *
     * \param list
     *         List to which to add the element.
     * \param element
     *         Element to add to the list (mostly \c this).
     */
    inline ImplicitList(T*& list, T* element) : mNext(list)
    {
        list = element;
    }

    /**
     * Get next element in the list.
     *
     * \return    Next element or zero if the end of the list is reached.
     */
    inline T*
    getNext()
    {
        return mNext;
    }

    /**
     * Remove an element from the list.
     *
     * \param head
     *         Head of the list.
     * \param element
     *         Element to remove.
     */
    static inline void
    removeFromList(T** head, const T* element)
    {
        if (*head != 0)
        {
            if (*head == element)
            {
                *head = element->mNext;
            }
            else
            {
                T* previous = *head;
                T* node = (*head)->mNext;

                while (node != element)
                {
                    previous = node;
                    node = node->mNext;
                }

                previous->mNext = node->mNext;  // cppcheck-suppress nullPointerRedundantCheck
            }
        }
    }

private:
    // disable copy constructor
    ImplicitList(const ImplicitList&);

    // disable assignment operator
    ImplicitList&
    operator=(const ImplicitList&);

    /// Pointer to the next element
    T* mNext;
};

}  // namespace outpost

#endif
