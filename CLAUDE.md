# CLAUDE.md

This file provides guidance to AI coding agents when working with code in this repository.

## Overview

SigViewer is a Qt 6 / C++17 desktop application for viewing and annotating biosignals (EEG, MEG, and similar time series). It reads signal files, displays channels in a scrolling browser, and lets users create, edit, and save events (annotations, artifact selections). File I/O relies on two third-party libraries: **libbiosig** (most biosignal formats, e.g. GDF/EDF/BDF) and **libxdf** (XDF streams). By default these are built from source and statically linked; see `SIGVIEWER_SYSTEM_DEPS` below to link dynamically against system packages instead.

## Build & test

Dependencies (`libxdf`, `libbiosig`) must be built once before the first CMake configure — they are installed into `external/` as static libraries:

```
cmake -P external/build_deps.cmake          # build/install pinned deps into external/
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(sysctl -n hw.logicalcpu)   # use $(nproc) on Linux
```

The build fails fast if `external/versions.cmake` is missing or its recorded versions don't match `LIBXDF_VERSION` / `LIBBIOSIG_VERSION` in `CMakeLists.txt`. Rebuild deps with `cmake -P external/build_deps.cmake` (add `-DFORCE_REBUILD=ON`, or `-DFORCE_REBUILD_LIBXDF=ON` / `-DFORCE_REBUILD_LIBBIOSIG=ON` to force one).

### Linking against system libxdf/libbiosig

`SIGVIEWER_SYSTEM_DEPS` (cache var: `AUTO` default / `ON` / `OFF`) controls whether libxdf/libbiosig are linked dynamically against system packages instead of the pinned static build above. `AUTO` tries `find_package(libxdf CONFIG)` plus the bundled `cmake/FindLibBiosig.cmake` module and falls back to the static path above if either is missing; `ON` requires both and fails configure otherwise; `OFF` (what release builds use) always uses the static path, skipping `find_package` entirely. Neither library's version can be verified automatically at configure time — libxdf has no `ConfigVersion.cmake` upstream, and libbiosig's header version macros are stale (the 3.9.5 release still reports 3.0.2) — so system packages are accepted as-is against the documented minimum in `CMakeLists.txt`.

Output: `build/SigViewer.app` (macOS), `build/sigviewer` (Linux), `build/sigviewer.exe` (Windows).

Tests (Qt Test framework, driven by CTest):

```
ctest --test-dir build -V                   # run all tests
ctest --test-dir build -R test_data_block   # run a single test by name
```

CTest sets `QT_QPA_PLATFORM=offscreen` automatically, so tests need no display. Test executables (`test_data_block`, `test_color_manager`, `test_event_manager`, `test_editing_commands`, `test_event_table_widget`, `test_file_handling`, `test_gui`) can also be run directly. Add a new test by calling `add_sigviewer_test(<name> src/tests/<name>.cpp)` in `CMakeLists.txt`.

## Dependency version pinning

`LIBXDF_VERSION` and `LIBBIOSIG_VERSION` are defined in **two** places that must stay in sync: near the top of `CMakeLists.txt` and near the top of `external/build_deps.cmake`. Update both when upgrading, then re-run the deps build.

## Translations

UI strings live in `src/translations/*.ts`. After changing translatable strings, regenerate with `cmake --build build --target sigviewer_lupdate` and commit the `.ts` files. The `.qm` binaries are compiled automatically during the build.

## Build structure

Nearly all source compiles into a single CMake `OBJECT` library, `sigviewer_objects` (defined by the big `qt_add_library` in `CMakeLists.txt`). Both the `sigviewer` executable and every test target link against it. **New `.cpp`/`.h` files must be added to that `qt_add_library` source list** or they won't compile. Qt AUTOMOC/AUTOUIC/AUTORCC are enabled; `.ui` and `.qrc` files are picked up automatically.

## Architecture

### Context objects (ownership hierarchy)

State is held in a tree of `QSharedPointer`-managed context objects:

- `ApplicationContext` (`src/application_context.h`) — process-wide singleton (`getInstance()`). Owns the `MainWindowModel`, the event `ColorManager`, and the current `FileContext`/`TabContext`. **Note: only one file open at a time** — `addFileContext` replaces the current one (multi-file is not implemented).
- `FileContext` (`src/file_context.h`) — per-open-file state: the reader, channel/event managers, dirty state.
- `TabContext` (`src/tab_context.h`) — per-tab state including its `CommandExecuter` (undo/redo stack).

Objects communicate via Qt signals carrying state enums (`ApplicationState`, `FileState`, `TabSelectionState`, `TabEditState`, defined under `src/base/*_states.h`). GUI actions enable/disable themselves in response to these state-change signals.

### Self-registering factories (the core extension pattern)

Three subsystems use the same idiom: a singleton factory keyed by string, populated at static-init time by registrator objects declared via macros. To add a new handler, drop in a source file and add one registration line — no central switch to edit.

- **File readers** — `FileSignalReaderFactory` keyed by file extension. A reader subclasses `FileSignalReader` (`src/file_handling/file_signal_reader.h`) and registers via `FILE_SIGNAL_READER_REGISTRATION(ext, Class)` / `FILE_SIGNAL_READER_DEFAULT_REGISTRATION(Class)` (macros in `file_handler_factory_registrator.h`). Current readers: `BiosigReader` (default/most formats) and `XDFReader`. Readers implement `createInstance()`, `getSignalData()`, `getEvents()`, `getBasicHeader()`.
- **File writers** — `FileSignalWriterFactory`, registered with `FILE_SIGNAL_WRITER_REGISTRATION(ext, Class)`.
- **GUI actions** — `GuiActionCommand` subclasses (in `src/gui/commands/`) register a `GuiActionFactoryRegistrator` (usually a static member). `main.cpp` calls `GuiActionFactoryRegistrator::registerActions()` at startup to instantiate them. Each command declares its action IDs, wires shortcuts/icons, implements `trigger()`, and overrides `updateEnabledness*` to react to state changes.

### Editing = undo/redo commands

All mutations to events/channels go through `QUndoCommand` subclasses in `src/editing_commands/` (e.g. `change_channel_undo_command`, `delete_event_undo_command`), pushed onto the tab's `CommandExecuter`. Prefer this path over mutating managers directly so undo/redo stays consistent.

### Signal display path

`FileSignalReader` → channel managers (`src/file_handling/`: `ChannelManager`, `FileChannelManager`, and decorators like `DetrendChannelManager`, plus a `DownsamplingThread` and `SignalCache` for performance) → `SignalVisualisationModel` / signal-browser view widgets (`src/gui/signal_browser/`). Events are held by `EventManager` and rendered/edited through the event view and `src/gui/event_table/`.

## Releasing

Bump `VERSION` in `CMakeLists.txt`, commit, then tag `v<version>` and push with `--tags`. Pushing the tag triggers `.github/workflows/release.yml`, which builds all platforms, packages artifacts, and publishes a GitHub release. Remember to update the download links in `README.md`. macOS release builds must be code-signed, notarized, and stapled (see `release.yml` for exact commands).
