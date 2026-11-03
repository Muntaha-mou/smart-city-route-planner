// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.

#include "core/project_info.h"

#ifndef SCP_VERSION
#define SCP_VERSION "0.0.0"
#endif

namespace scp {

std::string applicationName()
{
    return "Smart City Route & Infrastructure Planner";
}

std::string teamName()
{
    return "Quad Core";
}

std::string version()
{
    return SCP_VERSION;
}

std::string copyrightNotice()
{
    return "Copyright © 2026 Quad Core. All rights reserved.";
}

std::string projectDescription()
{
    return "A desktop application that models a city transportation network as a "
           "weighted graph and applies classic algorithms to plan routes, analyse "
           "connectivity and select infrastructure projects under a limited budget.";
}

} // namespace scp
