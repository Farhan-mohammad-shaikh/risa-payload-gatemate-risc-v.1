/*
 * Copyright (c) 2014-2019, Fabian Greif
 * Copyright (c) 2020, 2022, Jan-Gerd Mess
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#ifndef UNITTEST_CONFIGURABLE_EVENT_LISTENER_H
#define UNITTEST_CONFIGURABLE_EVENT_LISTENER_H

#include <gtest/gtest.h>

#include <stdio.h>

// exported from gtest.cc to provide colored output
namespace testing
{
namespace internal
{
enum class GTestColor
{
    kDefault,
    kRed,
    kGreen,
    kYellow
};

void
ColoredPrintf(GTestColor color, const char* fmt, ...);
}  // namespace internal
}  // namespace testing

namespace unittest
{
class ConfigurableEventListener : public testing::TestEventListener
{
public:
    inline explicit ConfigurableEventListener(TestEventListener* theEventListener) :
        showTestCases(false),
        showTestNames(false),
        showSuccesses(false),
        showInlineFailures(true),
        showEnvironment(false),
        eventListener(theEventListener)
    {
    }

    virtual ~ConfigurableEventListener();

    virtual void
    OnTestProgramStart(const testing::UnitTest& unit_test)
    {
        eventListener->OnTestProgramStart(unit_test);
    }

    virtual void
    OnTestIterationStart(const testing::UnitTest& unit_test, int iteration)
    {
        eventListener->OnTestIterationStart(unit_test, iteration);
        printf("\n");
    }

    virtual void
    OnEnvironmentsSetUpStart(const testing::UnitTest& unit_test)
    {
        if (showEnvironment)
        {
            eventListener->OnEnvironmentsSetUpStart(unit_test);
        }
    }

    virtual void
    OnEnvironmentsSetUpEnd(const testing::UnitTest& unit_test)
    {
        if (showEnvironment)
        {
            eventListener->OnEnvironmentsSetUpEnd(unit_test);
        }
    }

    virtual void
    OnTestCaseStart(const testing::TestCase& test_case)
    {
        if (showTestCases)
        {
            eventListener->OnTestCaseStart(test_case);
        }
    }

    virtual void
    OnTestStart(const testing::TestInfo& test_info)
    {
        if (showTestNames)
        {
            eventListener->OnTestStart(test_info);
        }
    }

    virtual void
    OnTestPartResult(const testing::TestPartResult& result)
    {
        eventListener->OnTestPartResult(result);
    }

    virtual void
    OnTestEnd(const testing::TestInfo& test_info);

    virtual void
    OnTestCaseEnd(const testing::TestCase& test_case)
    {
        if (showTestCases)
        {
            eventListener->OnTestCaseEnd(test_case);
        }
    }

    virtual void
    OnEnvironmentsTearDownStart(const testing::UnitTest& unit_test)
    {
        if (showEnvironment)
        {
            eventListener->OnEnvironmentsTearDownStart(unit_test);
        }
    }

    virtual void
    OnEnvironmentsTearDownEnd(const testing::UnitTest& unit_test)
    {
        if (showEnvironment)
        {
            eventListener->OnEnvironmentsTearDownEnd(unit_test);
        }
    }

    virtual void
    OnTestIterationEnd(const testing::UnitTest& unit_test, int iteration)
    {
        eventListener->OnTestIterationEnd(unit_test, iteration);
    }

    virtual void
    OnTestProgramEnd(const testing::UnitTest& unit_test)
    {
        eventListener->OnTestProgramEnd(unit_test);
    }

    /**
     * Show the names of each test case.
     */
    bool showTestCases;
    /**
     * Show the names of each test.
     */
    bool showTestNames;
    /**
     * Show each success.
     */
    bool showSuccesses;
    /**
     * Show each failure as it occurs. You will also see it at the bottom after the full
     * suite is run.
     */
    bool showInlineFailures;
    /**
     * Show the setup of the global environment.
     */
    bool showEnvironment;

protected:
    TestEventListener* eventListener;
};

ConfigurableEventListener*
registerConfigurableEventListener(int argc = 0, char** argv = nullptr);

}  // namespace unittest

#endif  // UNITTEST_CONFIGURABLE_EVENT_LISTENER_H
