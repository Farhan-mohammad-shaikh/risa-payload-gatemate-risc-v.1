/*
 * Copyright (c) 2017, Fabian Greif
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef OUTPOST_UTILS_BASE_MEMBER_PAIR_H
#define OUTPOST_UTILS_BASE_MEMBER_PAIR_H

namespace outpost
{
namespace utils
{
/**
 * Used to implement the EBCO (Empty Base Class Optimization) pattern.
 *
 * Merge two classes in the same address space if the base class is an
 * empty class.
 *
 * See the following article for more information:
 * http://www.informit.com/articles/article.aspx?p=31473&seqNum=2
 *
 * \author  Fabian Greif
 */
template <typename Base, typename Member>
class BaseMemberPair : private Base
{
public:
    BaseMemberPair(Base const& b, Member const& m) : Base(b), mMember(m)
    {
    }

    const Base&
    getBase() const
    {
        return reinterpret_cast<const Base&>(*this);
    }

    Base&
    getBase()
    {
        return reinterpret_cast<Base&>(*this);
    }

    const Member&
    getMember() const
    {
        return mMember;
    }

    Member&
    getMember()
    {
        return mMember;
    }

private:
    Member mMember;
};

}  // namespace utils
}  // namespace outpost

#endif
