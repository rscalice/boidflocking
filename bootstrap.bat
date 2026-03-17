@echo off
setlocal ENABLEDELAYEDEXPANSION

REM Change to script directory
cd /d "%~dp0"

echo [1/5] Initializing git submodule for Qt5...
git submodule update --init --recursive external/qt5
IF ERRORLEVEL 1 (
    echo Failed to init Qt5 submodule.
    exit /b 1
)

cd external\qt5

echo.
echo [2/5] Running Qt init-repository (this may take a while)...
perl init-repository
IF ERRORLEVEL 1 (
    echo Failed to run init-repository.
    exit /b 1
)

cd /d "%~dp0"

echo.
echo [3/5] Configuring CMake for Visual Studio 2022...
cmake -S . -B build -G "Visual Studio 17 2022"
IF ERRORLEVEL 1 (
    echo CMake configuration failed.
    exit /b 1
)

echo.
echo [4/5] Building Qt and the app (this will take a long time on first run)...
cmake --build build --config Release
IF ERRORLEVEL 1 (
    echo Build failed.
    exit /b 1
)

echo.
echo [5/5] Done. Open build\BoidFlockingQtSuperbuild.sln in Visual Studio 2022.

endlocal