/*
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_COMM_RMAP_NODE_LIST_H_
#define OUTPOST_COMM_RMAP_NODE_LIST_H_

#include "rmap_common.h"
#include "rmap_node.h"

#include <outpost/base/slice.h>

#include <stdint.h>
#include <string.h>

namespace outpost
{
namespace comm
{

/**
 * RMAP target node list.
 *
 * Provides the list for RMAP targets to be used by the RMAP initiator class.
 *
 * \author  Muhammad Bassam
 */
class RmapTargetsList
{
public:
    RmapTargetsList();
    explicit RmapTargetsList(outpost::Slice<RmapTargetNode*> rmapTargetNodes);
    ~RmapTargetsList();

    /**
     * Add a predefined RMAP target node into the list of pointers. A node should
     * be statically defined which is not destroyed after any context switch.
     *
     * \param node
     *      Pointer to the RMAP target node specifications
     *
     * \return
     *      True: node added to the list. False: List is already full
     * */
    bool
    addTargetNode(RmapTargetNode* node);

    /**
     * Add a predefined RMAP target nodes into the list
     *
     * \param nodes
     *      Array object of pointers to the RMAP target nodes
     *
     * \return
     *      True: nodes added to the list. False: too many nodes supplied
     * */
    bool
    addTargetNodes(outpost::Slice<RmapTargetNode*> nodes);

    /**
     * Get RMAP target node from the list
     *
     * \param name
     *      A simple string name of the node, specified earlier
     *
     * \return
     *      Pointer to the node if found, otherwise nullptr
     * */
    RmapTargetNode*
    getTargetNode(const char* name);

    /**
     * Get RMAP target node from the list
     *
     * \param name
     *      Target logical address
     *
     * \return
     *      Pointer to the node if found, otherwise nullptr
     * */
    RmapTargetNode*
    getTargetNode(uint8_t logicalAddress);

    //--------------------------------------------------------------------------
    inline uint8_t
    getSize() const
    {
        return mSize;
    }

    inline outpost::Slice<RmapTargetNode*>
    getTargetNodes()
    {
        return outpost::Slice<RmapTargetNode*>::unsafe(mNodes, mSize);
    }

private:
    // TODO Replace with bounded array
    RmapTargetNode* mNodes[rmap::maxNumberOfTargetNodes];
    uint8_t mSize;
};
}  // namespace comm
}  // namespace outpost

#endif /* OUTPOST_COMM_RMAP_NODE_LIST_H_ */
