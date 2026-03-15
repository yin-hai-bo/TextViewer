# Repository Guidelines

## Project Structure & Module Organization
This repository is a small Qt Widgets application built from the top level. Core application sources live in the repository root: `main.cpp` starts the app, `mainwindow.*` owns the main UI flow, `viewer.*`, `recentlist.*`, and `config.*` hold supporting behavior, and dialog classes such as `aboutbox.*` and `lineheightdialog.*` are split into `.h`, `.cpp`, and `.ui` files. Static assets and Windows resources live under `res/`. Build output should stay in [`build/`](C:\dev\QT\TextViewer\build), which is generated and should not contain hand-edited source.

## Build, Test, and Development Commands
Use CMake with Qt 5 or Qt 6 installed.

```powershell
cmake -S . -B build
cmake --build build --config Release
```

The first command configures the project; the second builds the `TextViewer` executable. For local iteration, `cmake --build build --config Debug` is the usual debug build. If you work in Qt Creator, open [`CMakeLists.txt`](C:\dev\QT\TextViewer\CMakeLists.txt) directly so `.ui`, `.ts`, and resource files stay wired into the build.

## Coding Style & Naming Conventions
Follow the existing C++ style in the repo: 4-space indentation, opening braces on the next line for functions, and include ordering with project headers before related Qt/system cleanup only when needed. Class names use `PascalCase` (`MainWindow`, `RecentList`), member fields use a trailing underscore (`recentList_`), and file names are lowercase (`viewer.cpp`, `config.h`). Prefer modern C++17 and Qt types already used in the codebase.

## Testing Guidelines
There is currently no automated test suite or `tests/` directory. Until tests are added, validate changes by building cleanly and exercising the affected UI flow manually in a debug build. If you introduce testable logic, prefer adding Qt Test-based coverage in a dedicated `tests/` target and name files after the unit under test, for example `test_recentlist.cpp`.

## Commit & Pull Request Guidelines
Recent history uses short Conventional Commit prefixes such as `feat:` and `fix:`. Keep commit subjects imperative and specific, for example `fix: preserve recent file ordering`. Pull requests should describe the user-visible change, list manual verification steps, and include screenshots when modifying dialogs, menus, or translated UI text.

## Configuration Notes
Do not commit machine-specific files such as `CMakeLists.txt.user` changes unless explicitly required. Keep translations in [`TextViewer_zh_CN.ts`](C:\dev\QT\TextViewer\TextViewer_zh_CN.ts) in sync when changing visible strings.
