/*
 * Copyright (c) 2014-2017, Fabian Greif
 * Copyright (c) 2015, Janosch Reinking
 * Copyright (c) 2016, Jan Sommer
 * Copyright (c) 2020, Jan Malburg
 * Copyright (c) 2022, Adrian Roeser
 * Copyright (c) 2022, Jan-Gerd Mess
 * Copyright (c) 2023, Pascal Pieper
 * Copyright (c) 2023, Kirstein
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_LIST_H
#define OUTPOST_LIST_H

#include <stddef.h>
#include <stdio.h>

namespace outpost
{
template <typename T>
class List;

class ListElement
{
    template <typename T>
    friend class List;

public:
    ListElement() = default;

    // Copying/Assignment does not put a element in a queue or removes it
    ListElement(const ListElement&)
    {
        mNext = nullptr;
    }

    ListElement&
    // cppcheck-suppress operatorEqVarError
    operator=(const ListElement&)
    {
        return *this;
    }

private:
    ListElement* mNext = nullptr;
};

/**
 * Singly-linked list with external storage.
 *
 * The nodes must provide a "mNext"-member.
 *
 * \author    Fabian Greif
 */
template <typename T>
class List
{
public:
    /**
     * Construct an empty list.
     */
    List();

    /**
     * Destructor.
     *
     * \warning
     *     The items contained in the list are not destroyed by deleting
     *     the list. The list does not take ownership of items added to it,
     *     therefore all items have to be destroyed by their original creator.
     */
    ~List();

    /**
     * Remove all entries from the list.
     *
     * The nodes are not changed only the link to the first node is
     * removed!
     *
     * O(1)
     */
    void
    reset();

    /**
     * Check if the list is empty.
     */
    bool
    isEmpty() const;

    /**
     * Get first element of the list.
     *
     * O(1)
     */
    T*
    first();

    const T*
    first() const;

    /**
     * Get last element of the list.
     *
     * O(1)
     */
    T*
    last();

    /**
     * Get last element of the list.
     *
     * O(1)
     */
    const T*
    last() const;

    /*
     * Get node that satisfies given condition
     */
    template <typename Condition>
    T*
    get(Condition condition);

    /**
     * Get the nth element of the list.
     *
     * O(N)
     */
    T*
    getN(size_t n);

    /*
     * Get node that satisfies given condition
     */
    template <typename Condition>
    const T*
    get(Condition condition) const;

    /**
     * Get the nth element of the list.
     *
     * O(N)
     */
    const T*
    getN(size_t n) const;

    /**
     * Add a node to the front of the list.
     *
     * O(1)
     */
    void
    prepend(T* node);

    /**
     * Add a node to the back of the list.
     *
     * O(1)
     */
    void
    append(T* node);

    /**
     * Insert a node sorted into the list.
     *
     * The class T has to be comparable via operator<. This comparison
     * shall produce a strict weak ordering of the elements.
     *
     * O(N)
     */
    void
    insert(T* node);

    /**
     * Remove a node from the list.
     *
     * As this is a singly linked list this method as to iterate through
     * the list to find the predecessor.
     *
     * O(N)
     *
     * \retval \c true if the node was found and removed from list,
     * \retval \c false if the node is not in the list.
     */
    bool
    removeNode(T* node);

    /**
     * Remove the first node that satisfy the given condition.
     *
     * O(N)
     *
     * \return  Node that has been removed from the list.
     */
    template <typename Condition>
    T*
    remove(Condition condition);

    /**
     * Remove all nodes that satisfy the given condition.
     *
     * O(N)
     */
    template <typename Condition>
    void
    removeAll(Condition condition);

    /**
     * Remove all nodes that satisfy the given condition and apply
     * a postcondition to the removed nodes.
     *
     * O(N)
     */
    template <typename Condition, typename PostCondition>
    void
    removeAll(Condition condition, PostCondition postCondition);

    /**
     * O(N)
     */
    size_t
    size() const;

    /**
     * Remove the first node from the list.
     *
     * O(1)
     */
    void
    removeFirst();

    class Iterator
    {
    public:
        friend class List;
        friend class ConstIterator;

        Iterator();

        Iterator(const Iterator& other);

        Iterator&
        operator=(const Iterator& other);

        Iterator&
        operator++();

        bool
        operator==(const Iterator& other) const;

        bool
        operator!=(const Iterator& other) const;

        T&
        operator*();

        T*
        operator->();

        explicit
        operator T*();

    private:
        explicit Iterator(T* node);

        /// Pointer to the current node. Set to NULL if end of list.
        T* mNode;
    };

    class ConstIterator
    {
    public:
        friend class List;

        ConstIterator();

        ConstIterator(const ConstIterator& other);

        ConstIterator(const Iterator& other);

        ConstIterator&
        operator=(const ConstIterator& other);

        ConstIterator&
        operator++();

        bool
        operator==(const ConstIterator& other) const;

        bool
        operator!=(const ConstIterator& other) const;

        const T&
        operator*() const;

        const T*
        operator->() const;

        explicit
        operator const T*() const;

    private:
        explicit ConstIterator(const T* node);

        /// Pointer to the current node. Set to NULL if end of list.
        const T* mNode;
    };

    Iterator
    begin();

    Iterator
    end();

    ConstIterator
    begin() const;

    ConstIterator
    end() const;

private:
    ListElement* mHead;
    ListElement* mTail;

    // disable copy constructor
    List(const List&);

    // disable assignment operator
    List&
    operator=(const List&);
};
}  // namespace outpost

#include "list_impl.h"

#endif
