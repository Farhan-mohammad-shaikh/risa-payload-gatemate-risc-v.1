/*
 * Copyright (c) 2024, Pieper, Pascal
 * Copyright (c) 2024, Cold, Erin Cynthia
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include <outpost/container/ring_buffer.h>
#include <outpost/container/ring_buffer_allocators/array_ring_buffer.h>

#include <unittest/container/ring_buffer_allocators/persistent_file_backed_ring_buffer.h>
#include <unittest/container/ring_buffer_helper.h>
#include <unittest/harness.h>

using namespace outpost;
using namespace outpost::container;
using namespace ::testing;

/**
 * \brief This more a demo than a test.
 */

TEST(ArrayRingBuffer, shouldCreateRingBufferWithInternalStorage)
{
    static constexpr auto capacity = 1024U;
    static ArrayRingBufferStorage<capacity, uint8_t> ringBuffer;

    ASSERT_THAT(ringBuffer.isEmpty(), IsTrue());
    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(0U));

    uint8_t buffer[100U] = {42U};
    ringBuffer.append(asSlice(buffer));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(100U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 100U));

    const auto res = ringBuffer.pop(10U);
    ASSERT_THAT(res.getNumberOfElements(), Eq(10U));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(90U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 90U));
    ASSERT_THAT(res[0], Eq(42));
}

TEST(ArrayRingBuffer, shouldCreateTypedRingBufferWithInternalStorage)
{
    using valueType = uint16_t;
    static constexpr auto capacity = 1024U;
    static ArrayRingBufferStorage<capacity, valueType> ringBuffer;

    ASSERT_THAT(ringBuffer.isEmpty(), IsTrue());
    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(0U));

    valueType buffer[100U] = {4242U};
    ringBuffer.append(asSlice(buffer));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(100U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 100U));

    const auto res = ringBuffer.pop(10U);
    ASSERT_THAT(res.getNumberOfElements(), Eq(10U));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(90U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 90U));
    ASSERT_THAT(res[0], Eq(4242));
}

TEST(ArrayRingBuffer, shouldCreateRingBufferWithExternalStorage)
{
    static constexpr auto capacity = 1024U;
    static uint8_t extBuffer[capacity];
    ArrayRingBuffer<uint8_t> ringBuffer(asSlice(extBuffer));

    ASSERT_THAT(ringBuffer.isEmpty(), IsTrue());
    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(0U));

    uint8_t buffer[100U] = {42U};
    ringBuffer.append(asSlice(buffer));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(100U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 100U));
    ASSERT_THAT(extBuffer[0U], Eq(42));

    const auto res = ringBuffer.pop(10U);
    ASSERT_THAT(res.getNumberOfElements(), Eq(10U));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(90U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 90U));
    ASSERT_THAT(res[0], Eq(42));
}

TEST(ArrayRingBuffer, shouldCreateTypedRingBufferWithExternalStorage)
{
    using valueType = uint16_t;
    static constexpr auto capacity = 1024U;
    static valueType extBuffer[capacity];
    ArrayRingBuffer<valueType> ringBuffer(asSlice(extBuffer));

    ASSERT_THAT(ringBuffer.isEmpty(), IsTrue());
    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(0U));

    valueType buffer[100U] = {42U};
    ringBuffer.append(asSlice(buffer));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(100U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 100U));
    ASSERT_THAT(extBuffer[0U], Eq(42));

    const auto res = ringBuffer.pop(10U);
    ASSERT_THAT(res.getNumberOfElements(), Eq(10U));

    ASSERT_THAT(ringBuffer.getAvailableElements(), Eq(90U));
    ASSERT_THAT(ringBuffer.getFreeElements(), Eq(capacity - 90U));
    ASSERT_THAT(res[0], Eq(42));
}

TEST(ArrayRingBuffer, arrayRingBufferStorageShouldInheritFromNormalRingBuffer)
{
    static constexpr auto capacity = 1024U;
    static ArrayRingBufferStorage<capacity, uint8_t> ringBufferStorage;

    ArrayRingBuffer<uint8_t>* referenceToPassAround = &ringBufferStorage;

    ASSERT_THAT(ringBufferStorage.isEmpty(), IsTrue());
    ASSERT_THAT(referenceToPassAround->isEmpty(), IsTrue());

    uint8_t buffer[10U] = {42U};
    ringBufferStorage.append(asSlice(buffer));

    auto res = referenceToPassAround->pop(10U);
    ASSERT_THAT(res.getNumberOfElements(), Eq(10U));
    ASSERT_THAT(res[0], Eq(42));
}

TEST(PersistentRingBuffer, implementsOperations)
{
    using PersistentAllocator = unittest::container::PersistentFileAllocator<1>;
    using namespace ring_buffer::detail;
    EXPECT_TRUE(ImplementsAllocator<PersistentAllocator>::mandatoryFunctions);
    EXPECT_FALSE(ImplementsDirectAccess<PersistentAllocator>::directAccess);
    EXPECT_TRUE(ImplementsMetadataAccess<PersistentAllocator>::value);
}

TEST(PersistentRingBuffer, PersistentRingBufferCanBeWrittenAndRead)
{
    static constexpr auto capacity = 300;
    static std::string filename = "persistentRingBuffer.bin";
    static constexpr size_t numIterations = 3;
    static constexpr size_t bytesPerIteration = 10;
    static_assert(numIterations * bytesPerIteration <= capacity, "Test setup invalid");

    if (std::fstream(filename))
    {
        // file already exists, delete it to have clear test env
        std::remove(filename.c_str());
    }

    unittest::container::PersistentFileBackedRingBuffer<capacity> persistentRingBuffer(filename);
    {
        std::array<uint8_t, bytesPerIteration> buffer;

        for (size_t i = 0; i < numIterations; i++)
        {
            std::fill(buffer.begin(), buffer.end(), i);
            persistentRingBuffer.append(asSlice(buffer));
        }
    }

    // Is file created and still there?
    ASSERT_THAT(std::fstream(filename), IsTrue());

    // This does _not_ create a new instance
    {
        std::array<uint8_t, bytesPerIteration> buffer;
        ASSERT_THAT(persistentRingBuffer.getAvailableElements(),
                    Eq(numIterations * bytesPerIteration));

        for (size_t i = 0; i < numIterations; i++)
        {
            ASSERT_THAT(persistentRingBuffer.popInto(asSlice(buffer)),
                        Eq(RingBufferBase::OperationResult::success));

            for (const auto& elem : buffer)
            {
                EXPECT_EQ(elem, i);
            }
        }
    }

    // clean up after test
    ASSERT_THAT(std::fstream(filename), IsTrue());
    std::remove(filename.c_str());
}

TEST(PersistentRingBuffer, PersistentRingBufferCanBeWrittenAndReadAfterRestart)
{
    static constexpr auto capacity = 300;
    static std::string filename = "persistentRingBuffer.bin";
    static constexpr size_t numIterations = 3;
    static constexpr size_t bytesPerIteration = 10;
    static_assert(numIterations * bytesPerIteration <= capacity, "Test setup invalid");

    if (std::fstream(filename))
    {
        // file already exists, delete it to have clear test env
        std::remove(filename.c_str());
    }

    {
        unittest::container::PersistentFileBackedRingBuffer<capacity> persistentRingBuffer(
                filename);

        std::array<uint8_t, bytesPerIteration> buffer;

        for (size_t i = 0; i < numIterations; i++)
        {
            std::fill(buffer.begin(), buffer.end(), i);
            persistentRingBuffer.append(asSlice(buffer));
        }
    }

    // Is file created and still there?
    ASSERT_THAT(std::fstream(filename), IsTrue());

    // this creates a new instance, which might as well be a restart of the whole system
    {
        unittest::container::PersistentFileBackedRingBuffer<capacity> persistentRingBuffer(
                filename);
        ASSERT_THAT(persistentRingBuffer.getAvailableElements(),
                    Eq(numIterations * bytesPerIteration));

        std::array<uint8_t, bytesPerIteration> buffer;

        for (size_t i = 0; i < numIterations; i++)
        {
            ASSERT_THAT(persistentRingBuffer.popInto(asSlice(buffer)),
                        Eq(RingBufferBase::OperationResult::success));

            for (const auto& elem : buffer)
            {
                EXPECT_EQ(elem, i);
            }
        }
    }

    // clean up after test
    ASSERT_THAT(std::fstream(filename), IsTrue());
    std::remove(filename.c_str());
}
