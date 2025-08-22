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

#include "rmap_node_list.h"

#include "rmap_status.h"  // for debug printing

using namespace outpost::comm;

RmapTargetsList::RmapTargetsList() : mNodes(), mSize(0)
{
}

RmapTargetsList::~RmapTargetsList()
{
}

RmapTargetsList::RmapTargetsList(outpost::Slice<RmapTargetNode*> rmapTargetNodes) : mSize{0}
{
    if (rmapTargetNodes.getNumberOfElements() <= rmap::maxNumberOfTargetNodes)
    {
        mSize = rmapTargetNodes.getNumberOfElements();
        addTargetNodes(rmapTargetNodes);
    }
}

bool
RmapTargetsList::addTargetNode(RmapTargetNode* node)
{
    bool result = false;

    if (mSize < rmap::maxNumberOfTargetNodes)
    {
        mNodes[mSize++] = node;
        result = true;
    }
    return result;
}

bool
RmapTargetsList::addTargetNodes(outpost::Slice<RmapTargetNode*> nodes)
{
    bool result = false;
    size_t listElements = nodes.getNumberOfElements();

    if ((listElements + mSize) <= rmap::maxNumberOfTargetNodes)
    {
        for (size_t i = 0; i < listElements; i++)
        {
            addTargetNode(nodes[i]);
        }
        result = true;
    }
    return result;
}

RmapTargetNode*
RmapTargetsList::getTargetNode(const char* name)
{
    if (nullptr == name)
    {
        return nullptr;
    }

    RmapTargetNode* rt = nullptr;

    for (uint8_t i = 0; i < mSize; i++)
    {
        if (!strncmp(mNodes[i]->getName(), name, rmap::maxNodeNameLength))
        {
            rt = mNodes[i];
            break;
        }
    }

    return rt;
}

RmapTargetNode*
RmapTargetsList::getTargetNode(uint8_t logicalAddress)
{
    RmapTargetNode* rt = nullptr;

    for (uint8_t i = 0; i < mSize; i++)
    {
        if (mNodes[i]->getTargetLogicalAddress() == logicalAddress)
        {
            rt = mNodes[i];
            break;
        }
    }

    if (!rt)
    {
        console_out("Error: No such RMAP target node\n");
    }

    return rt;
}
