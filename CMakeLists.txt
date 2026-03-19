cmake_minimum_required(VERSION 3.21)
project(BoidFlocking)

set(Qt6_DIR external/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6)

# Find the Qt6 package with required components
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGL OpenGLWidgets)
find_package(OpenGL REQUIRED)

# Set up standard Qt project settings
qt_standard_project_setup()

file(GLOB APP_SOURCES "app/*.cpp" "app/*.c" "app/*.h" "app/*.ui")

# Add the executable and link to Qt6::Widgets
add_executable(BoidFlocking ${APP_SOURCES})

target_include_directories(BoidFlocking PUBLIC ${CMAKE_SOURCE_DIR}/external/opencl/src/OpenCL/include/)

target_link_directories(BoidFlocking PRIVATE ${CMAKE_SOURCE_DIR}/external/opencl/src/OpenCL/lib/)
target_link_libraries(BoidFlocking Qt6::Widgets Qt6::Core Qt6::Gui Qt6::OpenGLWidgets OpenCL  ${OPENGL_LIBRARIES})

# Find the windeployqt executable
find_program(WINDEPLOYQT_EXECUTABLE NAMES windeployqt6 windeployqt PATHS ${Qt6_BIN_DIR} ENV PATH REQUIRED)

# Add as a post-build event
add_custom_command(
    TARGET BoidFlocking POST_BUILD
    COMMAND ${WINDEPLOYQT_EXECUTABLE}
    # Pass the path to your executable as an argument
    ARGS "$<TARGET_FILE:BoidFlocking>"
    # Optional arguments, e.g., --always-overwrite, --pdb, --release
    # ARGS --release --always-overwrite
    # Set the working directory to ensure correct environment
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:BoidFlocking>"
    COMMENT "Running windeployqt6.exe on the executable"
)