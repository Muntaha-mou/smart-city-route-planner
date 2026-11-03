// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.

#ifndef SCP_CORE_PROJECT_INFO_H
#define SCP_CORE_PROJECT_INFO_H

#include <string>

namespace scp {

// General information about the project, shared by the GUI and the tests.
std::string applicationName();
std::string teamName();
std::string version();
std::string copyrightNotice();
std::string projectDescription();

} // namespace scp

#endif // SCP_CORE_PROJECT_INFO_H
