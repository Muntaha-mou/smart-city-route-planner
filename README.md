# Smart City Route & Infrastructure Planner

A C++17 / Qt 6 desktop application that models a city transportation network as a
weighted graph and applies classic algorithms to plan routes, analyse connectivity
and select infrastructure projects under a limited budget.

**Team:** Quad Core
**Course:** CSE 232 — Algorithms project

> **Status:** Milestone 1 (project foundation) is complete. Algorithms and the full
> application screens are added in later milestones.

## Technology Stack

- C++17 and the Standard Template Library
- Qt 6 Widgets (desktop interface)
- CMake + Ninja (build system)
- CTest (test runner)
- CSV files (data storage)

## Project Structure

```
CMakeLists.txt        Build configuration
include/              Header files
  core/               Core layer (no Qt dependency)
  ui/                 Qt user interface
src/                  Source files (same layout as include/)
resources/            Qt resources (application stylesheet)
data/                 CSV data files
tests/                Test executables run by CTest
docs/                 Project documentation
```

The project is split into two parts:

- **`scp_core`** — a static library with the data models, graph, CSV handling and
  algorithms. It uses only standard C++, so it can be tested without the GUI.
- **`SmartCityPlanner`** — the Qt Widgets desktop application, which uses `scp_core`.

## Prerequisites (Windows 10/11)

Install the following with the **Qt Online Installer**
(<https://www.qt.io/download-open-source>, free Qt account required).
Choose **Custom installation** and select:

| Installer section | Component |
|---|---|
| Qt → Qt 6.12.0 | MinGW 15.1.0 64-bit |
| Qt → Developer and Designer Tools | MinGW 15.1.0 64-bit |
| | CMake |
| | Ninja |

The commands below assume Qt is installed in `S:\Qt`. If you installed it elsewhere
(for example `C:\Qt`), replace `S:` with your drive.

> **Note:** If Windows *Smart App Control* is turned on, it blocks programs you compile
> yourself. Check **Windows Security → App & browser control → Smart App Control**.

## Build and Run

Open **PowerShell** in the project folder and run:

```powershell
# 1. Use the Qt MinGW toolchain for this terminal session
$env:PATH = "S:\Qt\Tools\mingw1510_64\bin;S:\Qt\Tools\CMake_64\bin;S:\Qt\Tools\Ninja;$env:PATH"

# 2. Configure
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=S:/Qt/6.12.0/mingw_64

# 3. Build
cmake --build build

# 4. Run the tests
ctest --test-dir build --output-on-failure

# 5. Start the application
.\build\SmartCityPlanner.exe
```

The build copies the required Qt DLLs (using `windeployqt`) and the `data` folder next
to `SmartCityPlanner.exe`, so the program can also be started by double-clicking it.

The default build type is **Release**. For debugging, add `-DCMAKE_BUILD_TYPE=Debug`
to the configure command.

## Testing

Tests use a small built-in `CHECK(...)` helper (`tests/test_utils.h`) instead of an
external framework. Each test executable is registered with CTest and returns a
non-zero exit code if any check fails.

---

Copyright © 2026 Quad Core. All rights reserved.
