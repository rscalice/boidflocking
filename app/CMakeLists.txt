cmake_minimum_required(VERSION 3.21)
project(BoidFlockingQtApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# QT_INSTALL_DIR is defined in the superbuild
set(CMAKE_PREFIX_PATH ${QT_INSTALL_DIR})

find_package(Qt5 REQUIRED COMPONENTS Widgets)

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/MainWindow.ui
)

add_executable(app ${SOURCES})

target_link_libraries(app PRIVATE Qt5::Widgets)

set_target_properties(app PROPERTIES
    WIN32_EXECUTABLE TRUE
)