/*
 * Copyright (c) 2019, Fabian Greif
 * Copyright (c) 2024, Pieper, Pascal
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/storage/serialize.h>
#include <outpost/storage/serialize_storage_traits.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace outpost;

namespace test
{
enum class ControlMode : uint8_t
{
    automatic = 1,
    manual = 2
};
}

namespace outpost
{
using namespace test;

// Use uint8_t as storage type for `ControlMode`
template <>
struct SerializeBigEndianTraits<ControlMode> : public SerializeStorageTraits<ControlMode, uint8_t>
{
};
}  // namespace outpost

TEST(SerializeStorageTraitsTest, store)
{
    // Prepare stream in which the data should be written.
    std::array<uint8_t, 20> buffer;
    Serialize stream(outpost::asSlice(buffer));

    // Stores the enum as a single byte.
    stream.store<ControlMode>(ControlMode::manual);

    EXPECT_EQ(1, stream.getPosition());
    EXPECT_EQ(2, buffer[0]);
}
