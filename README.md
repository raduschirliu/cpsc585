# CPSC 585 Game

## Requirements
- CMake v3.21.0+
- Visual Studio Community Edition 2019 + "Desktop C++ Development" package/workflow
    - This must be the latest version (16.11.23 as of writing this)

**Note:** The build process in theory _should_ also work for MacOS/Unix, but some more work will be required to build PhysX and ensure it links properly.

## Setup
1. Pull and init Git submodules
```sh
# Run in repo root
git submodule update --init --recursive
```

2. Setup and Build PhysX
```sh
# If running from Git Bash / MinGW / etc.
.\setup_physx.bat
```

```bat
: If running from Windows command prompt
start setup_physx.bat
```

3. Setup CMake

3a. Command line
Either open the root repo folder in VSCode and select the "VS2019 Debug" preset, or run
```
# Run in the repo root
cmake --preset "VS2019 Debug"
```

3b. VSCode
    - Open the root folder in VSCode, and accept the extension suggestions
    - Restart VSCode if prompted
    - Allow CMake to configure the project
    - Project can be built & started from the 'Debug' panel

3c. For Visual Studio:
    - Open the solution file for this project under `build/cpsc585.sln`

## Building
From the command line, the CMake project can be built using:
```sh
# Run in repo root, assuming your build folder is named "build"
cmake --build build
```

The executable and build artifacts should all be in the folder:
```
{REPO_ROOT}/build/bin/[Debug/Release]
```

**The executable must be ran from the directory it was placed in, otherwise you will get errors for missing
DLLs/files/etc.**