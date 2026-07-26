@echo off
setlocal

:: Run from the tests directory so relative paths keep working.
pushd "%~dp0"

set "BUILD_DIR=%~dp0.build"
set "TEST_EXE=%BUILD_DIR%\Debug\unit_tests.exe"
set "GTEST_SOURCE=%~dp0.deps\googletest"
set "GTEST_INSTALL=%~dp0.deps\gtest-install"

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%~dp0.deps" mkdir "%~dp0.deps"

if not exist "%GTEST_INSTALL%\lib\cmake\GTest\GTestConfig.cmake" (
    echo Building GoogleTest dependency into %GTEST_INSTALL%...
    if not exist "%GTEST_SOURCE%" (
        git clone https://github.com/google/googletest.git "%GTEST_SOURCE%"
        if errorlevel 1 exit /b %errorlevel%
    )

    cmake -S "%GTEST_SOURCE%" -B "%GTEST_SOURCE%\build" -DCMAKE_INSTALL_PREFIX="%GTEST_INSTALL%" -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=ON -Dgtest_build_tests=OFF
    if errorlevel 1 exit /b %errorlevel%

    cmake --build "%GTEST_SOURCE%\build" --config Debug
    if errorlevel 1 exit /b %errorlevel%

    cmake --install "%GTEST_SOURCE%\build" --config Debug
    if errorlevel 1 exit /b %errorlevel%
)

echo Building unit tests...
cmake -S . -B "%BUILD_DIR%" -DGTest_DIR="%GTEST_INSTALL%\lib\cmake\GTest"
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --config Debug --target unit_tests
if errorlevel 1 exit /b %errorlevel%

if exist "%TEST_EXE%" (
    echo Running unit tests...
    "%TEST_EXE%"
    if errorlevel 1 exit /b %errorlevel%
) else (
    echo Unit test executable not found at "%TEST_EXE%"
    exit /b 1
)

:: Undo the "push" command.
popd
endlocal
