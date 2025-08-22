/*
 * Copyright (c) 2019, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/base/enum_range_trait.h>

#include <unittest/harness.h>

enum class TestEnum : uint8_t
{
    value1 = 1,
    value2 = 2,
    value3 = 3,
    value4 = 4
};

namespace outpost
{
template <>
struct EnumRangeTrait<TestEnum>
{
    static constexpr TestEnum
    min()
    {
        return TestEnum::value1;
    }

    static constexpr TestEnum
    max()
    {
        return TestEnum::value4;
    }

    static constexpr bool
    isValid(const TestEnum& t)
    {
        return outpost::isValidEnum(t);
    }
};
}  // namespace outpost

TEST(EnumRangeTest, testMin)
{
    TestEnum e = TestEnum::value1;
    EXPECT_EQ(e, outpost::EnumRangeTrait<TestEnum>::min());
}

TEST(EnumRangeTest, testMax)
{
    TestEnum e = TestEnum::value4;
    EXPECT_EQ(e, outpost::EnumRangeTrait<TestEnum>::max());
}

TEST(EnumRangeTest, testValid)
{
    TestEnum e = TestEnum::value2;
    EXPECT_TRUE(outpost::EnumRangeTrait<TestEnum>::isValid(e));

    e = static_cast<TestEnum>(5U);
    EXPECT_FALSE(outpost::EnumRangeTrait<TestEnum>::isValid(e));
}
