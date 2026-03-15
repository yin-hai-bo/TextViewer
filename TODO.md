# TODO

## Milestone 1: Stability

- [ ] Add `MainWindow` flow tests for startup, empty state, and close behavior.
- [ ] Add file-opening tests for normal text, empty files, missing files, and large files.
- [ ] Add UI-level recent file tests for insertion, deduplication, and reopening from the menu.
- [ ] Expand config restore tests for window geometry, line height, font, and language.
- [ ] Add a translation smoke test to verify key UI text updates after language switches.
- [ ] Keep test persistence isolated from machine-specific settings backends.
- [ ] Run the relevant `Debug` test suite before each submission.

## Milestone 2: Release Readiness

- [ ] Expand `README.md` with overview, dependencies, build, run, test, and screenshots.
- [ ] Document the supported matrix for Qt versions, Windows versions, and compilers.
- [ ] Define a single source of truth for the application version.
- [ ] Add a repeatable packaging flow for the executable, Qt runtime, translations, and resources.
- [ ] Validate first-run behavior on a clean machine or clean user profile.
- [ ] Write a release checklist covering build, test, package, and smoke validation.
- [ ] Add a `CHANGELOG.md` or a release note template.

## Milestone 3: Practical Features

- [ ] Add Find support with next, previous, and current-match highlighting.
- [ ] Add Go To Line support.
- [ ] Add encoding handling for UTF-8, GBK/GB18030, and UTF-16, with selection or detection.
- [ ] Add display options such as word wrap, tab width, and read-only status feedback.
- [ ] Improve large-file loading so the UI remains responsive.
- [ ] Refine error messages for open, decode, and read failures.
- [ ] Evaluate whether a Reload File action is needed.

## Milestone 4: Structure

- [ ] Review `MainWindow` responsibilities and split file loading, action wiring, and state restore concerns where needed.
- [ ] Centralize config keys and related constants.
- [ ] Unify menu, action, shortcut, and translation string management.
- [ ] Keep a clear distinction between logic tests, widget tests, and window flow tests.
- [ ] Simplify the CMake structure for the app and test targets.
- [ ] Require either automated tests or explicit manual verification steps for new features.

## Suggested Order

1. Add `MainWindow` flow tests.
2. Add recent file UI tests.
3. Finish config restore coverage.
4. Improve release documentation and packaging.
5. Add Find support.
6. Add Go To Line support.
7. Add encoding handling.
