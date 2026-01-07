# FileRenamer

## Description

**FileRenamer** is a desktop application developed in C++ with the Qt framework, designed to simplify bulk file renaming and creation of PDF files with custom names. It offers an intuitive user interface and customization options such as theme selection (light or dark) and numbering pattern configuration. A fun virus simulation feature (can be disabled) is also included for demonstrations.

## Features

- **File Renaming**:
  - Selection of a source folder and a destination folder.
  - Definition of a base name for renamed files.
  - Customization of numbering patterns (number of digits, adding letters before or after, custom regex).
- **PDF File Creation**:
  - Generation of empty PDF files with a base name and sequential numbering.
  - Choice of destination folder and number of files (from 1 to 10,000).
- **Customizable Settings**:
  - Theme selection (light or dark).
  - Configuration of the number of digits for numbering.
  - Pattern options for renaming (with or without letters, custom regex).
- **Virus Simulation (optional)**:
  - After 5 minutes, multiple pop-ups open simulating a virus alert (can be disabled in settings).
- Modern interface with a splash screen at startup.

## Executable Version

- The project includes a compiled and executable version in the `bin` folder. You can download this folder to use the application without having to compile it yourself.
- On Windows, run `FileRenamer.exe` to start the application.
- Make sure all dependencies (Qt DLLs, etc.) are present in the `bin` folder for the application to work correctly.

## Installation

### Prerequisites

Before starting, make sure you have installed:
- **CMake** (version 3.16 or higher) to generate the project.
- **Qt** (version 5 or 6) with the **Widgets** component.
- A compatible C++ compiler (for example, MSVC on Windows, GCC on Linux/macOS).
- A development environment (optional, such as Qt Creator or Visual Studio).

### Installation Steps

#### Download and Preparation

1. Download the project ZIP from the GitHub repository and extract it to a folder (for example, `FileRenamer-master`).
2. Rename the folder to `FileRenamer` to simplify paths (optional).

#### Configuration and Compilation

1. Open a terminal in the `FileRenamer` folder.
2. Create a build folder:
   ```
   mkdir build
   cd build
   ```
3. Configure the project with CMake (adjust the Qt path according to your installation):
   ```
   cmake .. -DCMAKE_PREFIX_PATH="path/to/Qt/5.x.x/compiler/lib/cmake"
   ```
   Example on Windows with Qt 5.15.2 and MSVC:
   ```
   cmake .. -DCMAKE_PREFIX_PATH="C:/Qt/5.15.2/msvc2019_64/lib/cmake"
   ```
4. Compile the project:
   ```
   cmake --build . --config Release
   ```
5. The executable `FileRenamer` (or `FileRenamer.exe` on Windows) will be generated in `build/Release`.

#### Launching the Application

1. Navigate to the `build/Release` folder.
2. Launch the application:
   - On Windows: double-click on `FileRenamer.exe`.
   - On Linux/macOS: run `./FileRenamer` in the terminal.
3. A splash screen will display for 1.3 seconds, followed by the main window.

**Note:** Recompile the project if you modify the code or resources (for example, `resources.qrc`).

## Usage

1. **Rename Files**:
   - Click "Browse" to select the source folder and destination folder.
   - Enter a base name for the files.
   - Click "Rename" to start the process.
2. **Create PDF Files**:
   - Enter a base name for the PDF files.
   - Select the destination folder.
   - Specify the number of files to create (1 to 10,000).
   - Click "Create PDF Files".
3. **Settings**:
   - Adjust the number of digits for numbering.
   - Choose a renaming pattern (letters before/after, custom regex).
   - Select a theme (light or dark).
   - Enable/disable the virus simulation (enabled by default).

## Project Structure

The project is organized as follows:
- **`resources.qrc`**: Contains resources such as:
  - `icon.png`: Application icon.
  - `style.qss`: Style for the dark theme.
  - `light.qss`: Style for the light theme.
  - `animation.gif`: Animation (not currently used).
- **`mainwindow.ui`**: Interface designed with Qt Designer (tabs "Rename", "Create PDF", "Settings").
- **`CMakeLists.txt`**: CMake script for configuration and compilation.
- **`app.rc`**: Windows resources for the icon (Windows only).
- **`mainwindow.h` and `mainwindow.cpp`**: Main logic (events, settings, virus simulation).
- **`main.cpp`**: Entry point, initializes the application and splash screen.
- **`splashwidget.h` and `splashwidget.cpp`**: Splash screen management.

## Troubleshooting Common Issues

- **CMake Error During Compilation**:
  - Check the Qt path in `CMAKE_PREFIX_PATH`.
  - Make sure the **Widgets** component is installed.
- **The Application Doesn't Start or Themes Don't Apply**:
  - Verify that `style.qss` and `light.qss` are included in `resources.qrc`.
- **The Virus Simulation Doesn't Work**:
  - Confirm that the option is enabled in "Settings" and wait 5 minutes.
- **The "Browse" Buttons Don't Respond**:
  - Check the slot connections in `mainwindow.cpp`.
