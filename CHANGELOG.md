## [UNRELEASED] · YYYY-MM-DD
### ✨ Added
- Use SVG icons with support for light and dark themes

## [0.6.5] · 2026-03-24
### 🌀 Changed
- Switch from Qt 5 to Qt 6
- Switch to CMake
- Building works again on Windows (in addition to Linux and macOS)

### 🔧 Fixed
- Fix crash when exporting events to CSV
- Fix EVT import when sampling rates differ

## [0.6.4] · 2019-04-23
### ✨ Added
- Re-enable import/export of events from/to EVT

## [0.6.3] · 2019-03-21
### 🔧 Fixed
- Update libxdf (improves stability)

## [0.6.2] · 2018-07-12
### ✨ Added
- Store user-defined colors
- Use effective sample rate in XDF files

### 🌀 Changed
- Refactoring and code cleanups

### 🔧 Fixed
- Fix issues in some dialogs
- Fix channel clipping

## [0.6.1] · 2017-04-26
### 🌀 Changed
- Several minor GUI-related improvements

### 🔧 Fixed
- Fix many XDF-related bugs

## [0.6.0] · 2017-03-09
### ✨ Added
- Add support for XDF files
- Export and import events to/from CSV files

### 🌀 Changed
- Redesign view options widget
- New icon theme
- Switch from Qt 4 to Qt 5
- Many UI improvements (channel separators, grids, dialogs)

### 🔧 Fixed
- Properly handle missing data

## [0.5.2]
### 🌀 Changed
- Integrate patches from the BioSig project

## [0.5.1]
### 🌀 Changed
- Events are now always displayed in a separate tab

### 🗑️ Removed
- Remove libgdf
- Remove downsampling/decimation

### 🔧 Fixed
- Fix bug where signals could be distorted
- Fix file drag & drop bug

## [0.5.0]
### 🌀 Changed
- Optimize GDF reading (background downsampling/decimation)
- Support for large GDF2 files
- Use libgdf to read GDF2 files

### 🔧 Fixed
- Fix memory leak

## [0.4.3]
### 🌀 Changed
- Switch to new libgdf
- Remove dependency on boost library (only headers are needed)

### 🔧 Fixed
- Fix crash on Windows XP when deselecting channels

## [0.4.2]
### ✨ Added
- Convert to GDF (using libgdf)
- Command-line options (e.g. `--convert-to-gdf`, `--help`)
- Improve y-grid drawing
- Improve y-grid labels (displaying units)
- Improve y-scaling, new dialog

### 🌀 Changed
- Power spectrum is now displayed in log10
- Switch from FFTW to FFTReal
- Refactoring and code cleanups

### 🔧 Fixed
- Fix instant viewport update after color changing and scaling
- Fix crash when signal processing leads to overflow

## [0.4.1]
### ✨ Added
- Animated event browsing
- Reset event colors
- Setting default channel color

### 🌀 Changed
- Speed up animations (duration is now settable via the View menu)
- Rename "Shift Signal Mode" to "View Options Mode"

### 🔧 Fixed
- Fix various bugs

## [0.4.0]
### ✨ Added
- Animated zoom
- Export to GDF
- Export to PNG
- Calculate power spectrum averaged over an event type
- Calculate mean and standard deviation averaged over an event type
- Color settings for signal channels
- Mode-specific widgets (for editing selected events)
- New context menu for channels

### 🌀 Changed
- Works on macOS again
- Improve dialogs (event table, event type selection)
- Refactoring and code clean-ups

### 🔧 Fixed
- Fix many bugs

## [0.3.0]
### ✨ Added
- Drop files on SigViewer to open them
- Open files via command line parameter
- Event toolbar (for editing selected events)
- Calculate mean (alpha version)
- Undo/redo for editing events
- Insert event (Ctrl+I), creates new event over the selected event
- Event browsing (Ctrl+Right, Ctrl+Left), go to next/previous event of same type as selected event
- Fit view to selected event
- Hide events of other type
- Highlight on x-axis when editing or creating an event
- New context menu for events
- Simple hiding/showing of label and axis widgets

### 🌀 Changed
- Remove all Qt 3 code, now completely ported to Qt 4
- Refactoring and code clean-ups (especially decoupling of classes)

### 🔧 Fixed
- Fix many bugs

## [0.2.6]
### ✨ Added
- Qt Creator support
- Signal buffer options (whole subsampling selection, disable initial min-max search)

### 🌀 Changed
- Clean up code in biosig_reader.cpp

### 🔧 Fixed
- Fix many bugs related to loading files (including slow loading over Windows network shares)

## [0.2.5]
### ✨ Added
- Preferences dialog

### 🌀 Changed
- Remove some Qt 3 code

## [0.2.4]
### ✨ Added
- Update biosig4c++ to support more file formats

## [0.2.3]
### ✨ Added
- Improve support for BCI2000 1.1 data format

## [0.2.2]
### ✨ Added
- Add new file formats (BCI2000, MIT/Physiobank)
- Improve handling of some file formats (EDF+, GDF1, BrainVision, AINF)

### 🔧 Fixed
- Fix export and import of EVT files
- Fix missing file name in window title

## [0.2.1]
### 🔧 Fixed
- Fix CNT and VHDR errors

## [0.2.0]
### ✨ Added
- Add support for biosig4c++; loading and saving is now done with this library, supporting many new formats (GDF, BKR, CNT, EDF, ...)
- Scrolling during event creation is now possible
- Support for big-endian machines (e.g. PPC)

### 🌀 Changed
- Standard zoom setting is now auto
- Zero level on the y-axis is now in the middle of the channel window

### 🔧 Fixed
- Fix channel selection

## [0.1.7]
### 🔧 Fixed
- Fix export of EVT files with a sampling rate higher than 256 Hz

## [0.1.6]
### 🔧 Fixed
- Fix crash when opening a file (probably due to a Windows update)

## [0.1.5]
### 🔧 Fixed
- Fix crash on exit

## [0.1.4]
### 🔧 Fixed
- Fix bug in SignalBuffer

## [0.1.3]
### ✨ Added
- y-axis

### 🌀 Changed
- Replace SettingsManager with QSettings

## [0.1.2]
### ✨ Added
- Status bar with length, number of channels, and number of trials
- Export events to EVT files
- Show all channels after opening a file

### 🔧 Fixed
- Fix signal shift bug
- Fix channel selection having no effect on scale

## [0.1.1]
### 🌀 Changed
- Use QString and Qt containers instead of STL

### 🔧 Fixed
- Fix line wrap in LogDialog
