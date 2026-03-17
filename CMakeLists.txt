cmake_minimum_required(VERSION 3.21)
project(BoidFlockingQtSuperbuild LANGUAGES CXX)

include(ExternalProject)

# Where Qt will be installed after building
set(QT_INSTALL_DIR ${CMAKE_BINARY_DIR}/qt-install)

# Allow user override from cache or command line
set(QT_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/qt5 CACHE PATH "Path to Qt5 source")

ExternalProject_Add(Qt5Project
    SOURCE_DIR ${QT_SOURCE_DIR}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure.bat
        -prefix ${QT_INSTALL_DIR}
        -opensource -confirm-license
        -nomake tests -nomake examples
        -platform win32-msvc
    BUILD_COMMAND nmake
    INSTALL_COMMAND nmake install
)

# Add the application subdirectory
add_subdirectory(app)

# Ensure the app builds after Qt
add_dependencies(app Qt5Project)