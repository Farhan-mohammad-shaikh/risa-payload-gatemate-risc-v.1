/*
 * Copyright (c) 2014-2019, Fabian Greif
 * Copyright (c) 2020, 2022, Jan-Gerd Mess
 * Copyright (c) 2020, Jan Malburg
 *
 * This file is part of the Simple Interface Protocols (SIP) examples.
 *
 * It is supplied solely for the use by TUHH and HAW Hamburg
 * in the frame of the PLUTO 1 flight mission.
 * Distribution outside of the project or to people with no share in the PLUTO mission requires explicit permit granted by DLR-RY-AVS
 * Contact jan-gerd.mess@dlr.de when in doubt.
 */

#include "configurable_event_listener.h"

namespace unittest
{
ConfigurableEventListener::~ConfigurableEventListener()
{
    delete eventListener;
}

void
ConfigurableEventListener::OnTestEnd(const testing::TestInfo& test_info)
{
    if (showInlineFailures && test_info.result()->Failed())
    {
        testing::internal::ColoredPrintf(testing::internal::GTestColor::kRed, "[  FAILED  ]");
        printf(" %s.%s\n\n", test_info.test_case_name(), test_info.name());
    }
    else if (showSuccesses && !test_info.result()->Failed())
    {
        eventListener->OnTestEnd(test_info);
    }
}

static bool
inArgumentList(int argc, char** argv, const char* str)
{
    for (int i = 0; i < argc; i++)
    {
        if (strcmp(argv[i], str) == 0)
        {
            return true;
        }
    }
    return false;
}

ConfigurableEventListener*
registerConfigurableEventListener(int argc, char** argv)
{
    // Remove the default listener
    testing::TestEventListeners& listeners = testing::UnitTest::GetInstance()->listeners();
    auto defaultPrinter = listeners.Release(listeners.default_result_printer());

    // Add our listener, by default everything is off, like:
    // [==========] Running 149 tests from 53 test cases.
    // [==========] 149 tests from 53 test cases ran. (1 ms total)
    // [ PASSED ] 149 tests.
    ConfigurableEventListener* listener = new ConfigurableEventListener(defaultPrinter);

    if (inArgumentList(argc, argv, "--verbose"))
    {
        listener->showTestNames = true;
        listener->showSuccesses = true;
    }

    listeners.Append(listener);

    return listener;
}
}  // namespace unittest
