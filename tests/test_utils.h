// Smart City Route & Infrastructure Planner
// Copyright © 2026 Quad Core. All rights reserved.
//
// A very small test helper used instead of a full testing framework.
// CHECK(condition) records a pass or a failure; finishTests() prints a summary
// and returns the process exit code that CTest uses (0 = all passed).

#ifndef SCP_TESTS_TEST_UTILS_H
#define SCP_TESTS_TEST_UTILS_H

#include <iostream>
#include <string>

namespace test {

struct Counters {
    int passed = 0;
    int failed = 0;
};

inline Counters& counters()
{
    static Counters instance;
    return instance;
}

inline void check(bool condition, const char* expression, const char* file, int line)
{
    if (condition) {
        ++counters().passed;
    } else {
        ++counters().failed;
        std::cout << "  FAILED: " << expression << "  (" << file << ":" << line << ")\n";
    }
}

inline void section(const std::string& name)
{
    std::cout << "[" << name << "]\n";
}

inline int finishTests()
{
    const Counters& c = counters();
    std::cout << "\n" << c.passed << " checks passed, " << c.failed << " failed.\n";
    return c.failed == 0 ? 0 : 1;
}

} // namespace test

#define CHECK(condition) test::check((condition), #condition, __FILE__, __LINE__)

#endif // SCP_TESTS_TEST_UTILS_H
