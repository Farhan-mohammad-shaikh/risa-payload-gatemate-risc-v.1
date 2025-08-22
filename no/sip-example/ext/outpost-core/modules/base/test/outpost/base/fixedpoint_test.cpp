/*
 * Copyright (c) 2019, 2022-2023, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/base/fixedpoint.h>

#include <unittest/harness.h>

#include <array>

// Instantiate template class and functions for correct lcov calculations
template class outpost::FP<16>;

template outpost::FP<16>
outpost::FP<16>::operator+(const double& x) const;

template outpost::FP<16>&
outpost::FP<16>::operator+=(const double& x);

template outpost::FP<16>
outpost::FP<16>::operator-(const double& x) const;

template outpost::FP<16>&
outpost::FP<16>::operator-=(const double& x);

template outpost::FP<16>
outpost::FP<16>::operator*(const double& x) const;

template outpost::FP<16>&
outpost::FP<16>::operator*=(const double& x);

template outpost::FP<16>
outpost::FP<16>::operator/(const double& x) const;

template outpost::FP<16>&
outpost::FP<16>::operator/=(const double& x);

template bool
outpost::FP<16>::operator<(const double& x) const;

template bool
outpost::FP<16>::operator>(const double& x) const;

template bool
outpost::FP<16>::operator<=(const double& x) const;

template bool
outpost::FP<16>::operator>=(const double& x) const;

template bool
outpost::FP<16>::operator==(const double& x) const;

template bool
outpost::FP<16>::operator!=(const double& x) const;

TEST(FixedpointTest, fixedpointDefaultContructors)
{
    outpost::Fixedpoint fp1;
    EXPECT_EQ(fp1.getValue(), 0);

    int16_t n16 = 135;
    outpost::Fixedpoint fp2(n16);
    EXPECT_EQ(fp2.getValue(), 135 << 16);

    int32_t n32 = -16000;
    outpost::Fixedpoint fp3(n32);
    EXPECT_EQ(fp3.getValue(), -1048576000);

    float f = 3.25f;
    outpost::Fixedpoint fp4(f);
    EXPECT_EQ(fp4.getValue(), 0x00034000);

    double d = 1.125;
    outpost::Fixedpoint fp5(d);
    EXPECT_EQ(fp5.getValue(), 0x00012000);

    outpost::Fixedpoint fp6(fp5);
    EXPECT_EQ(fp6.getValue(), 0x00012000);
}

TEST(FixedpointTest, castingOperators)
{
    double d = 1.125;
    outpost::Fixedpoint fp1(d);
    EXPECT_EQ(static_cast<int32_t>(fp1), 1);
    EXPECT_EQ(static_cast<int16_t>(fp1), 1);
    EXPECT_EQ(static_cast<float>(fp1), 1.125f);
    EXPECT_EQ(static_cast<double>(fp1), 1.125);

    d = 1.5;
    outpost::Fixedpoint fp2(d);
    EXPECT_EQ(static_cast<int32_t>(fp2), 2);
    EXPECT_EQ(static_cast<int16_t>(fp2), 2);
    EXPECT_EQ(static_cast<float>(fp2), 1.5f);
    EXPECT_EQ(static_cast<double>(fp2), 1.5);

    d = -1.125;
    outpost::Fixedpoint fp3(d);
    EXPECT_EQ(static_cast<int32_t>(fp3), -1);
    EXPECT_EQ(static_cast<int16_t>(fp3), -1);
    EXPECT_EQ(static_cast<float>(fp3), -1.125f);
    EXPECT_EQ(static_cast<double>(fp3), -1.125);

    d = -3.5;
    outpost::Fixedpoint fp4(d);
    EXPECT_EQ(static_cast<int32_t>(fp4), -4);
    EXPECT_EQ(static_cast<int16_t>(fp4), -4);
    EXPECT_EQ(static_cast<float>(fp4), -3.5f);
    EXPECT_EQ(static_cast<double>(fp4), -3.5);

    d = -38.0;
    outpost::Fixedpoint fp5(d);
    EXPECT_EQ(static_cast<double>(fp5), -38.0);
    EXPECT_EQ(static_cast<int16_t>(fp5), -38);

    d = -6.75;
    outpost::Fixedpoint fp6(d);
    EXPECT_EQ(static_cast<double>(fp6), -6.75);
    EXPECT_EQ(static_cast<int16_t>(fp6), -7);
}

TEST(FixedpointTest, assignmentOperators)
{
    double d = -1.75;
    float f = -2.5f;
    int16_t i16 = 125;
    int32_t i32 = -3;

    outpost::Fixedpoint fp;
    fp = d;
    EXPECT_EQ(static_cast<double>(fp), -1.75);

    fp = f;
    EXPECT_EQ(static_cast<double>(fp), -2.5);

    fp = i16;
    EXPECT_EQ(static_cast<double>(fp), 125.0);

    fp = i32;
    EXPECT_EQ(static_cast<double>(fp), -3.0);

    outpost::Fixedpoint fp2;
    fp2 = fp;
    EXPECT_EQ(static_cast<double>(fp2), -3.0);
}

TEST(FixedpointTest, additionOperators)
{
    double d = -1.75;
    float f = -2.5f;
    int16_t i16 = 125;
    int32_t i32 = -3;

    outpost::Fixedpoint fp1(17.5);
    outpost::Fixedpoint fp2(-16.5);

    outpost::Fixedpoint res = fp1 + fp2;
    EXPECT_EQ(static_cast<double>(res), 1.0);

    res += f;
    EXPECT_EQ(static_cast<double>(res), -1.5);

    res = fp1 + d;
    EXPECT_EQ(static_cast<double>(res), 15.75);

    res += f;
    EXPECT_EQ(static_cast<double>(res), 13.25);

    res += i16;
    EXPECT_EQ(static_cast<double>(res), 138.25);

    res = res + i32;
    EXPECT_EQ(static_cast<double>(res), 135.25);
}

TEST(FixedpointTest, subtractionOperators)
{
    double d = -1.75;
    float f = -2.5f;
    int16_t i16 = 125;
    int32_t i32 = -3;

    outpost::Fixedpoint fp1(17.5);
    outpost::Fixedpoint fp2(-16.5);

    outpost::Fixedpoint res = fp1 - fp2;
    EXPECT_EQ(static_cast<double>(res), 34.0);

    res -= f;
    EXPECT_EQ(static_cast<double>(res), 36.5);

    res = fp1 - d;
    EXPECT_EQ(static_cast<double>(res), 19.25);

    res -= f;
    EXPECT_EQ(static_cast<double>(res), 21.75);

    res -= i16;
    EXPECT_EQ(static_cast<double>(res), -103.25);

    res = res - i32;
    EXPECT_EQ(static_cast<double>(res), -100.25);
}

TEST(FixedpointTest, multiplicationOperators)
{
    double d = -1.75;
    float f = -2.5f;
    int16_t i16 = 125;
    int32_t i32 = -1;

    outpost::Fixedpoint fp1(17.5);
    outpost::Fixedpoint fp2(-16.5);

    outpost::Fixedpoint res = fp1 * fp2;
    EXPECT_EQ(static_cast<double>(res), -288.75);

    res *= f;
    EXPECT_EQ(static_cast<double>(res), 721.875);

    res = fp1 * d;
    EXPECT_EQ(static_cast<double>(res), -30.625);

    res *= f;
    EXPECT_EQ(static_cast<double>(res), 76.5625);

    res *= i16;
    EXPECT_EQ(static_cast<double>(res), 9570.3125);

    res = res * i32;
    EXPECT_EQ(static_cast<double>(res), -9570.3125);
}

TEST(FixedpointTest, divisionOperators)
{
    double d = -1.75;
    float f = -0.008f;
    int16_t i16 = 125;
    int32_t i32 = -3;

    outpost::Fixedpoint fp1(17.5);
    outpost::Fixedpoint fp2(-16.5);

    outpost::Fixedpoint res = fp1 / fp2;
    EXPECT_LE(static_cast<double>(res) + 1.06060, 0.0001);
    EXPECT_GE(static_cast<double>(res) + 1.06060, -0.0001);

    res /= f;
    EXPECT_LE(static_cast<double>(res) - 132.6469, 0.0001);
    EXPECT_GE(static_cast<double>(res) - 132.6469, -0.0001);

    res = fp1 / d;
    EXPECT_EQ(static_cast<double>(res), -10.0);

    res /= f;
    EXPECT_LE(static_cast<double>(res) - 1250.6870, 0.0001);
    EXPECT_GE(static_cast<double>(res) - 1250.6870, -0.0001);

    res /= i16;
    EXPECT_LE(static_cast<double>(res) - 10.0055, 0.0001);
    EXPECT_GE(static_cast<double>(res) - 10.0055, -0.0001);

    res = res / i32;
    EXPECT_LE(static_cast<double>(res) + 3.3351, 0.0001);
    EXPECT_GE(static_cast<double>(res) + 3.3351, -0.0001);
}

TEST(FixedpointTest, shiftOperator)
{
    int32_t t = 0xFFFF;
    outpost::Fixedpoint fp(t);

    fp >>= 1;
    EXPECT_EQ(fp.getValue(), static_cast<int32_t>(0xFFFF8000));

    fp = fp >> 16;
    EXPECT_EQ(fp.getValue(), static_cast<int32_t>(0xFFFFFFFF));

    fp <<= 1;
    EXPECT_EQ(fp.getValue(), static_cast<int32_t>(0xFFFFFFFE));

    fp = fp << 31;
    EXPECT_EQ(fp.getValue(), static_cast<int32_t>(0));

    outpost::Fixedpoint fp2(-1.5);
    fp2 <<= 3;
    EXPECT_EQ(static_cast<double>(fp2), -12.0);

    outpost::Fixedpoint fp3(1.5);
    fp3 <<= 3;
    EXPECT_EQ(static_cast<double>(fp3), 12.0);
}

TEST(FixedpointTest, setValue)
{
    outpost::Fixedpoint fp(125.0);
    fp.setValue(0xFFFF8000);
    EXPECT_EQ(static_cast<double>(fp), -0.5);
}

TEST(FixedpointTest, ltGtOperators)
{
    outpost::Fixedpoint fp1(123.0);
    outpost::Fixedpoint fp2(0.125);
    outpost::Fixedpoint fp3(-123.0);
    outpost::Fixedpoint fp4(-0.125);

    EXPECT_FALSE(fp1 < fp2);
    EXPECT_FALSE(fp1 < fp3);
    EXPECT_FALSE(fp2 < fp4);
    EXPECT_FALSE(fp4 < fp3);

    EXPECT_FALSE(fp1 < 0.0001);
    EXPECT_FALSE(fp1 < 122);
    EXPECT_FALSE(fp1 < 123);

    EXPECT_TRUE(fp1 > fp2);
    EXPECT_TRUE(fp1 > fp3);
    EXPECT_TRUE(fp2 > fp4);
    EXPECT_TRUE(fp4 > fp3);

    EXPECT_TRUE(fp1 > 0.0001f);
    EXPECT_TRUE(fp1 > 122);

    EXPECT_FALSE(fp1 > 123);
}

TEST(FixedpointTest, lessOrEqualOperators)
{
    outpost::Fixedpoint fp1(123.0);
    outpost::Fixedpoint fp2(0.125);
    outpost::Fixedpoint fp3(-123.0);
    outpost::Fixedpoint fp4(-0.125);

    EXPECT_FALSE(fp1 <= fp2);
    EXPECT_FALSE(fp1 <= fp3);
    EXPECT_FALSE(fp2 <= fp4);
    EXPECT_FALSE(fp4 <= fp3);

    EXPECT_FALSE(fp1 <= 0.0001);
    EXPECT_FALSE(fp1 <= 122);

    EXPECT_TRUE(fp1 >= fp2);
    EXPECT_TRUE(fp1 >= fp3);
    EXPECT_TRUE(fp2 >= fp4);
    EXPECT_TRUE(fp4 >= fp3);

    EXPECT_TRUE(fp1 >= 0.0001);
    EXPECT_TRUE(fp1 >= static_cast<int16_t>(122));
    EXPECT_TRUE(fp1 >= static_cast<int16_t>(123));
    EXPECT_TRUE(fp1 <= static_cast<int16_t>(123));
}

TEST(FixedpointTest, equalityOperators)
{
    outpost::Fixedpoint fp1(1.125);
    outpost::Fixedpoint fp2(1.135);

    EXPECT_TRUE(fp1 != fp2);
    EXPECT_TRUE(fp1 == 1.125);
    EXPECT_TRUE(fp2 == 1.135);
    EXPECT_TRUE(static_cast<double>(fp2) != 1.135);

    EXPECT_TRUE(fp1 != 1);
    EXPECT_TRUE(fp2 != 1.13);

    double d = 2.5;
    float f = 2.5f;
    outpost::Fixedpoint fp3(d);
    outpost::Fixedpoint fp4(f);

    EXPECT_EQ(fp3, fp4);
}

TEST(FixedpointTest, absTest)
{
    outpost::Fixedpoint fp1(1.125);
    outpost::Fixedpoint fp2(-1.678);

    EXPECT_EQ(fp1.abs(), fp1);
    EXPECT_EQ(fp2.abs(), fp2 * (-1.0));
}
