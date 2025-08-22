/*
 * Copyright (c) 2024, Janosch Reinking
 * Copyright (c) 2024, Tepe, Alexander
 * Copyright (c) 2024, Reinking, Janosch
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_RTOS_MOCK_H
#define UNITTEST_RTOS_MOCK_H

#include <outpost/container/list.h>
#include <outpost/time.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

namespace unittest
{
namespace rtos
{

class PeriodicTaskManagerMock;

template <typename, typename>
class Mockable;

class Ignore
{
public:
    template <typename T>
    operator T() const
    {
        return T();
    }
};

enum class MockMode
{
    replace,
    inspect,
};

static constexpr MockMode defaultMode = MockMode::replace;

// ---------------------------------------------------------------------------
/**
 * \class MockRaw
 */
class MockRaw
{
public:
    explicit MockRaw(MockMode mode);

    MockRaw(const MockRaw&) = delete;

    MockRaw&
    operator=(const MockRaw&) = delete;

    ~MockRaw() = default;

    MockMode
    getMode() const;

private:
    MockMode mMode;
};

// ---------------------------------------------------------------------------
/**
 * \class Mock
 *
 * Base class of all mocks.
 *
 * \tparam Mockable
 *      The class which will be mocked by the mock
 */
template <typename Mockable>
class Mock : public MockRaw
{
public:
    template <typename, typename>
    friend class unittest::rtos::Mockable;

    using Type = Mockable;

    explicit Mock(MockMode mode) : MockRaw(mode), mMockable(nullptr)
    {
    }

    Mock(const Mock&) = delete;

    Mock&
    operator=(const Mock&) = delete;

    ~Mock() = default;

    /**
     * Access the mockable of this mock
     *
     * \return the mockable
     */
    Mockable*
    getMockable() const
    {
        return mMockable;
    }

    void
    setMockable(Mockable* instance)
    {
        mMockable = instance;
    }

    auto*
    getUnderlyingObject()
    {
        Mockable* mockable = this->getMockable();
        assert(nullptr != mockable);
        return mockable->getUnderlyingObject();
    }

private:
    Mockable* mMockable;
};

// ---------------------------------------------------------------------------
/**
 * \class MockList
 *
 * Base class of all mockables
 */
template <typename Mock>
class MockList
{
public:
    static outpost::List<typename Mock::Type> allInstances;

    template <typename T>
    static void
    injectMock(const T& object, Mock& mock, const std::optional<size_t>& index = std::nullopt)
    {
        bool found = false;
        auto iter = allInstances.begin();
        size_t objectCounter = 0;
        while (iter != allInstances.end())
        {
            if (isInRange(object, *iter))
            {
                if (!iter->hasMock())
                {
                    if (index.has_value() && *index != objectCounter)
                    {
                        // there was an index given and it does not match.
                        break;
                    }
                    found = true;
                    iter->setMock(mock);
                    break;
                }
                ++objectCounter;
            }
            ++iter;
        }

        assert(found);
    }

    explicit MockList(typename Mock::Type* mockable) : mMock(nullptr), mMockable(mockable)
    {
        allInstances.append(mockable);
    }

    MockList(const MockList&) = delete;

    virtual ~MockList()
    {
        allInstances.removeNode(this->mMockable);
    }

    virtual void
    setMock(Mock& mock)
    {
        // mock was already injected
        assert(mMock == nullptr);
        mMock = &mock;
        mMock->setMockable(mMockable);
    }

    bool
    hasMock() const
    {
        return this->mMock != nullptr;
    }

protected:
    Mock* mMock;
    typename Mock::Type* mMockable;

private:
    template <typename T>
    static bool
    isInRange(const T& object, typename Mock::Type& mockable)
    {
        const auto startOfObject = reinterpret_cast<intptr_t>(&object);
        const auto endOfObject = startOfObject + static_cast<ptrdiff_t>(sizeof(T));
        return (mockable.getAddress() >= startOfObject) && (mockable.getAddress() < endOfObject);
    }
};

// ---------------------------------------------------------------------------
/**
 * \class Mockable
 *
 * Base class of all mockable implementations (Thread, Mutex, Semaphore, ...).
 *
 * \tparam Mock
 *      The Mock class related to the mockable
 */
template <typename Mock, typename UnderlyingType>
class Mockable : public MockList<Mock>, public outpost::ListElement
{
public:
    using MockType = Mock;

    explicit Mockable(typename Mock::Type* mockable,
                      intptr_t address,
                      UnderlyingType* underlyingObject) :
        MockList<Mock>(mockable), mAddress(address), mUnderlyingObject(underlyingObject)
    {
    }

    Mockable(const Mockable&) = delete;

    Mockable&
    operator=(const Mockable&) = delete;

    constexpr intptr_t
    getAddress() const
    {
        return mAddress;
    }

    template <typename R>
    static R
    handleMethodCall(const MockRaw* mock,
                     std::function<R()>& mockFunction,
                     std::function<R()>& realFunction)
    {
        if (mock != nullptr)
        {
            switch (mock->getMode())
            {
                case (MockMode::inspect): {
                    mockFunction();
                    return realFunction();
                }
                case (MockMode::replace): {
                    return mockFunction();
                }
                default: break;
            }
        }

        return realFunction();
    }

    UnderlyingType*
    getUnderlyingObject()
    {
        return this->mUnderlyingObject;
    }

protected:
    intptr_t mAddress;

private:
    UnderlyingType* mUnderlyingObject;
};

// ---------------------------------------------------------------------------
template <typename Mock>
outpost::List<typename Mock::Type> MockList<Mock>::allInstances;

// ---------------------------------------------------------------------------
/**
 * \brief Inject mock into object
 *
 * RTOS-Mocks can not be passed as references to the constructor of the unit
 * under test due the nature of the RTOS-module, which does not provide
 * interfaces and must be directly instantiated or must be used as super
 * classes. E.g.:
 *
 * unittest::rtos::ThreadMock mock;
 * UnitUnderTest myUnit;
 * unittest::rtos::injectMock(myUnit, mock);
 *
 * The code above will connect the thread of \c myUnit with the mock. I.e.:
 * all calls to the \c Thread API will be forwarded to the mock. All mockable
 * objects (Thread, Mutex, Queue, ...) will be managed in a static link list
 * (there is one list for each mockable type). In order to match the mock with
 * the mockable, the list will be traversed. The first mockable which exists in
 * the the address space of the object (here \c myUnit) and has not yet been
 * connect to a mock, will be used. Therefore the mockable will be determined
 * by:
 *
 * 1. the type of the mockable
 * 2. the address of the mockable
 * 3. the order of creation (first created will be used first)
 *
 * This function will raise an assertion if:
 *
 * 1. no mockable could be found
 * 2. the mock is already used
 *
 * \param object
 *      The object that contains the mockable. The mockable can be part of the
 *      private segment of the objects class. The start and end address of
 *      \p object will be used two determine the mockable.
 *
 * \param mock
 *      The mock to inject into \p object.
 *
 * \param index
 *      Optional parameter. If passed an index, `injectMock` will count
 *      all available mocks and only inject the mock into an object with
 *      the given index.
 */
template <typename T, typename U>
std::enable_if_t<std::is_base_of_v<unittest::rtos::MockRaw, U>, void>
injectMock(const T& object, U& mock, std::optional<size_t> index = std::nullopt)
{
    static_assert(!std::is_same_v<unittest::rtos::PeriodicTaskManagerMock, U>,
                  "Please use 'registerMock' instead");

    MockList<typename U::Type::MockType>::template injectMock<T>(object, mock, index);
}

// ---------------------------------------------------------------------------
/**
 * Same as \c injectMock. But uses creation order only to determine the
 * mockable. This function will only be used for the
 * \c PeriodicTaskManager. Since objects of this type must be created on stack.
 */
void
registerMock(unittest::rtos::PeriodicTaskManagerMock& mock);

}  // namespace rtos
}  // namespace unittest

#endif  // UNITTEST_RTOS_MOCK_H
