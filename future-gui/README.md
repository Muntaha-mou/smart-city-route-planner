# Future GUI (Qt 6) — parked prototype

This folder keeps the Qt 6 Widgets prototype created in milestone M1.
It is **not part of the active build**. The project is being developed as a
command-line (CLI) application first; this code is kept for a later Qt GUI.

## Contents

| File | Purpose |
|---|---|
| `src/main.cpp` | Qt entry point: font and stylesheet loading |
| `include/ui/main_window.h`, `src/ui/main_window.cpp` | Basic main window (header, welcome panel, status bar) |
| `resources/resources.qrc`, `resources/style.qss` | Qt resource file and stylesheet with the project colour palette |

The GUI will reuse the same graph and algorithm code as the CLI
(`include/graph.h`, `src/graph.cpp` and, later, the algorithm files).
Only the user-interface layer is replaced.

> **Note:** this prototype includes `core/project_info.h`, which no longer
> exists in the simplified CLI version. When reviving the GUI, replace the
> `scp::applicationName()`, `scp::teamName()`, `scp::version()`,
> `scp::copyrightNotice()` and `scp::projectDescription()` calls with plain
> text (for example `"Smart City Route & Infrastructure Planner"`).

## Re-enabling it later

The prototype was built and tested with **Qt 6.12.0 (MinGW 15.1.0 64-bit)**.
To build it again, move the files back under `include/ui`, `src/ui` and
`resources/` (or adjust the paths) and add this to the root `CMakeLists.txt`:

```cmake
find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(SmartCityPlannerGui WIN32
    future-gui/src/main.cpp
    future-gui/include/ui/main_window.h
    future-gui/src/ui/main_window.cpp
    future-gui/resources/resources.qrc
    src/graph.cpp
)
target_include_directories(SmartCityPlannerGui PRIVATE future-gui/include include)
target_link_libraries(SmartCityPlannerGui PRIVATE Qt6::Widgets)

# Copy Qt DLLs next to the executable
add_custom_command(TARGET SmartCityPlannerGui POST_BUILD
    COMMAND Qt6::windeployqt --no-translations --no-system-d3d-compiler
            --no-system-dxc-compiler --no-opengl-sw --verbose 0
            $<TARGET_FILE:SmartCityPlannerGui>)
```

Configure with the Qt path, for example:

```powershell
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=S:/Qt/6.12.0/mingw_64
```

The original version of these files is also in Git history (commit `78f875e`).

---

Copyright © 2026 Quad Core. All rights reserved.
