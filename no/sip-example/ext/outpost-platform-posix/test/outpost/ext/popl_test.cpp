/*
 * Copyright (c) 2017, German Aerospace Center (DLR)
 *
 * This file is part of the development version of OUTPOST.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * Authors:
 * - 2017, Jan Sommer (DLR SC-SRV)
 */
// ----------------------------------------------------------------------------

#include <popl.hpp>

#include <unittest/harness.h>

TEST(PoplTest, compilationTest)
{
    popl::OptionParser op("Compilation Test");

    bool bSwitch;
    auto switchOption = op.add<popl::Switch>("s", "switch", "test switch", &bSwitch);

    EXPECT_FALSE(switchOption->is_set());
    EXPECT_EQ(switchOption->count(), 0U);
}

TEST(PoplTest, parseTest)
{
    popl::OptionParser op("Parse Test");

    const char* argv[3] = {"filename", "-s", "-s"};

    bool bSwitch;
    auto switchOption = op.add<popl::Switch>("s", "switch", "test switch", &bSwitch);
    op.parse(3U, argv);

    EXPECT_TRUE(switchOption->is_set());
    EXPECT_EQ(switchOption->count(), 2U);
}
