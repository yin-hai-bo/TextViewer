# TextViewer

A simple Qt Widgets application created for learning Qt.

## Build

Build from the repository root with CMake and either Qt 5 or Qt 6 installed:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

For local development, a `Debug` build is usually more convenient:

```powershell
cmake -S . -B build
cmake --build build --config Debug
```

## Run Tests

Automated tests are built with CMake and run with `ctest`.

If you have not configured or built the project yet:

```powershell
cmake -S . -B build
cmake --build build --config Debug
ctest -C Debug --output-on-failure --test-dir build
```

If the `Debug` build is already up to date, run just the tests:

```powershell
ctest -C Debug --output-on-failure --test-dir build
```

To list discovered tests without running them:

```powershell
ctest -C Debug -N --test-dir build
```
