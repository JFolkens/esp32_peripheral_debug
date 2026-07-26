ESP-IDF Peripheral Debug Web Server
====================

Getting Started
---------------

This project is built with ESP-IDF for the ESP32 platform. Follow the steps below to set up your machine and build the project locally.

### 1. Install the required tools

Install the following on Windows:

- ESP-IDF Visual Studio Code extension
- ESP-IDF tools and Python environment (the extension will guide you through this)
- ESP32 USB drivers so Windows can detect the board

If you are using VS Code, the ESP-IDF extension provides the recommended setup flow and will install the needed toolchain components for you.

### 2. Open the project in VS Code

Open the repository root in Visual Studio Code and make sure the ESP-IDF extension is active for this workspace.

### 3. Configure the ESP-IDF environment

When prompted by the extension, select the ESP-IDF version and install the required tools. On Windows, this typically includes:

- Python
- Git
- CMake
- Xtensa toolchain components for ESP32

### 4. Build and flash the firmware

The ESP-IDF extension will add icons to VS Code for building, flashing, and monitoring the ESP32 device. The "fire" icon will do all three (build, flash, monitor).

### 5. Run the unit tests

The repository also includes a lightweight unit-test setup for local development. From the repository root, run:

- `tests\run_unit_tests.bat`

This builds the test target and runs the unit tests. The first run of the unit tests will also install and compile GoogleTests for Windows, which will take several minutes.

### 6. Optional: install pre-commit hooks

To run the unit tests and clang formatting automatically before commits, install the pre-commit hooks:

- `pre-commit install`

After that, the configured hook will run the unit tests before each commit.

