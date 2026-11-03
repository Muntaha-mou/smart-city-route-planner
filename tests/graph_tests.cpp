// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// Tests for the core layer (project information, graph and CSV handling).

#include "core/project_info.h"
#include "test_utils.h"

namespace {

void testProjectInfo()
{
    test::section("Project information");
    CHECK(scp::applicationName() == "Smart City Route & Infrastructure Planner");
    CHECK(scp::teamName() == "Quad Core");
    CHECK(!scp::version().empty());
    CHECK(scp::copyrightNotice() == "Copyright © 2026 Quad Core. All rights reserved.");
    CHECK(!scp::projectDescription().empty());
}

} // namespace

int main()
{
    testProjectInfo();
    return test::finishTests();
}
