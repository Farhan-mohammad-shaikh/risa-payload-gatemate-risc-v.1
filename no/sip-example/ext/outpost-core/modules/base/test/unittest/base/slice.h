/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_BASE_SLICE_H
#define UNITTEST_BASE_SLICE_H

#include <outpost/base/slice.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tuple>

namespace unittest
{
namespace base
{

// ---------------------------------------------------------------------------
/**
 * \class SliceEqMatcher
 */
template <typename Rhs>
class SliceEqMatcher
{
public:
    using is_gtest_matcher = void;

    explicit SliceEqMatcher(const Rhs& rhs) : mRhs(rhs)
    {
    }

    template <typename Lhs>
    bool
    MatchAndExplain(const Lhs& lhs, std::ostream* /* listener */) const
    {
        return lhs.isContentEqual(mRhs);
    }

    void
    DescribeTo(std::ostream* os) const
    {
        *os << "is equal to " << ::testing::PrintToString(mRhs);
    }

    void
    DescribeNegationTo(std::ostream* os) const
    {
        *os << "is not equal to " << ::testing::PrintToString(mRhs);
    }

private:
    const Rhs& mRhs;
};

template <typename Rhs>
inline SliceEqMatcher<Rhs>
SliceEq(const Rhs& rhs)
{
    return SliceEqMatcher<Rhs>(rhs);
}

// ---------------------------------------------------------------------------
/**
 * \class SliceSizeIsMatcher
 */
template <typename Inner>
class SliceSizeIsMatcher
{
public:
    using SizeType = decltype(std::declval<outpost::Slice<uint8_t>>().getNumberOfElements());

    explicit SliceSizeIsMatcher(Inner inner) : mInner(::testing::SafeMatcherCast<SizeType>(inner))
    {
    }

    template <typename Lhs>
    operator ::testing::Matcher<Lhs>() const
    {
        return ::testing::MakeMatcher(new Impl<Lhs>(mInner));
    }

private:
    template <typename Lhs>
    class Impl : public ::testing::MatcherInterface<Lhs>
    {
    public:
        explicit Impl(const ::testing::Matcher<SizeType>& internal) : mInner(internal)
        {
        }

        bool
        MatchAndExplain(Lhs value, ::testing::MatchResultListener* listener) const override
        {
            const std::string sizeAsString = ::testing::PrintToString(value.getNumberOfElements());
            *listener << "whose size is " << sizeAsString << " ";
            return mInner.MatchAndExplain(value.getNumberOfElements(), listener);
        }

        void
        DescribeTo(::std::ostream* os) const override
        {
            (*os) << "size ";
            mInner.DescribeTo(os);
        }

        void
        DescribeNegationTo(::std::ostream* os) const override
        {
            (*os) << "size ";
            mInner.DescribeNegationTo(os);
        }

    private:
        ::testing::Matcher<SizeType> mInner;
    };

    ::testing::Matcher<SizeType> mInner;
};

template <typename Inner>
inline SliceSizeIsMatcher<Inner>
SliceSizeIs(Inner inner)
{
    return SliceSizeIsMatcher<Inner>(inner);
}

// ---------------------------------------------------------------------------
// gMock Actions
ACTION_TEMPLATE(SliceCopyFrom, HAS_1_TEMPLATE_PARAMS(size_t, N), AND_1_VALUE_PARAMS(source))
{
    std::get<N>(args).copyFrom(source);
}

ACTION_TEMPLATE(SliceCopyInto, HAS_1_TEMPLATE_PARAMS(size_t, N), AND_1_VALUE_PARAMS(destination))
{
    destination.copyFrom(std::get<N>(args));
}

ACTION_TEMPLATE(SliceFirst, HAS_1_TEMPLATE_PARAMS(size_t, N), AND_1_VALUE_PARAMS(firstElements))
{
    return std::get<N>(args).first(firstElements);
}

ACTION_TEMPLATE(SliceGetNumberOfElements, HAS_1_TEMPLATE_PARAMS(size_t, N), AND_0_VALUE_PARAMS())
{
    return std::get<N>(args).getNumberOfElements();
}

}  // namespace base
}  // namespace unittest

#endif  // UNITTEST_BASE_SLICE_H
