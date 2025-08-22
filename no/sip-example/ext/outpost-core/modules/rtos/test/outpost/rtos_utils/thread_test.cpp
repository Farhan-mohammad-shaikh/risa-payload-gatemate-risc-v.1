#include <outpost/rtos_utils/looping_thread.h>
#include <outpost/rtos_utils/periodic_thread.h>
#include <outpost/rtos_utils/stoppable_thread.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <future>
#include <optional>
#include <thread>

using namespace std;
using namespace outpost::rtos_utils;
using namespace outpost::time;

template <typename ThreadType>
struct TestThread : public ThreadType
{
    static constexpr uint8_t defaultPriority = 1;

    // forward all parameters to derived Thread, and add `defaultPriority`
    template <typename... Args>
    explicit TestThread(const std::optional<size_t>& stepsUntilException, Args&&... args) :
        ThreadType{std::forward<Args>(args)..., defaultPriority},
        mStepsUntilException{stepsUntilException}
    {
    }

    // make run function public for testing
    using ThreadType::run;

private:
    // step function that will be called.
    // With exception to get out of the context
    void
    step() override
    {
        if (mStepsUntilException.has_value())
        {
            if (*mStepsUntilException == 0)
            {
                throw runtime_error("Expected Exception");
            }
            else
            {
                mStepsUntilException = *mStepsUntilException - 1;
            }
        }
    }

    optional<size_t> mStepsUntilException;
};

TEST(Threads, canBeInstantiated)
{
    TestThread<LoopingThread> looper{std::nullopt};
    TestThread<PeriodicThread> perioder{std::nullopt, Milliseconds{10}};
    TestThread<StoppableThread> stopper{std::nullopt};
    (void) looper;
    (void) perioder;
    (void) stopper;
}

TEST(Threads, looperLoops)
{
    static constexpr size_t iterations = 30000;
    TestThread<LoopingThread> looper{iterations};

    try
    {
        looper.run();
    }
    catch (const std::exception& e)
    {
        EXPECT_EQ(std::string{e.what()}, "Expected Exception");
        // Getting here successfully means that the
        // looper was executed `iterations` times.
    }
}

// This test leads to memory leaks on some system since -fno-exceptions
// (periodic_task_manager.cpp) and -fexceptions (thread_test.cpp) are
// mixed. This may break the RAII pattern since the destructors are not
// called in the code which is compiled with -fno-exceptions when a exception
// is thrown
TEST(Threads, DISABLED_perioderPeriods)
{
    const auto period_ms = 10;
    const size_t stopAfter = 100;
    TestThread<PeriodicThread> perioder{stopAfter, Milliseconds{period_ms}};

    const auto start{std::chrono::steady_clock::now()};
    try
    {
        perioder.run();
    }
    catch (const std::exception& e)
    {
        EXPECT_EQ(std::string{e.what()}, "Expected Exception");
    }
    const auto stop{std::chrono::steady_clock::now()};
    const std::chrono::duration<double> elapsedSeconds{stop - start};
    EXPECT_NEAR(elapsedSeconds.count(), (period_ms * stopAfter) / 1000., 0.02);
}

TEST(Threads, stopperStops)
{
    TestThread<StoppableThread> stopper{std::nullopt};

    std::packaged_task<void()> task([&stopper] { stopper.run(); });
    auto future = task.get_future();
    std::thread thread(std::move(task));

    EXPECT_NE(future.wait_for(20ms), std::future_status::ready);

    stopper.doStop();

    EXPECT_EQ(future.wait_for(20ms), std::future_status::ready);

    ASSERT_TRUE(thread.joinable());
    thread.join();
}
