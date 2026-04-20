cmake_minimum_required(VERSION 3.16)

project(ShiftStitch
    VERSION 1.0.0
    DESCRIPTION "Panorama stitching library using SIFT algorithm"
    LANGUAGES CXX
)

# -------------------------
# Options
# -------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(SHIFTSTITCH_BUILD_TESTS "Build tests" ON)
option(SHIFTSTITCH_BUILD_EXAMPLES "Build examples" ON)
option(SHIFTSTITCH_FETCH_OPENCV "Force download OpenCV" OFF)

# -------------------------
# C++ config
# -------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# -------------------------
# OpenCV
# -------------------------
if(ANDROID)
    message(STATUS "Android build detected")
    message(STATUS "You must provide OpenCV_DIR manually")

    find_package(OpenCV REQUIRED)

else()
    find_package(OpenCV QUIET)

    if(NOT OpenCV_FOUND AND SHIFTSTITCH_FETCH_OPENCV)
        message(STATUS "Fetching OpenCV (this may take a long time)...")
        include(cmake/FetchOpenCV.cmake)
        fetch_opencv()
    elseif(NOT OpenCV_FOUND)
        message(FATAL_ERROR
            "OpenCV not found.\n"
            "Install it OR enable downloading:\n"
            "  -DSHIFTSTITCH_FETCH_OPENCV=ON"
        )
    endif()
endif()

# -------------------------
# Library
# -------------------------
add_library(ShiftStitch SHARED
    src/shiftstitch.cpp
    src/sift.cpp
    src/result.cpp
)

target_include_directories(ShiftStitch
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(ShiftStitch
    PUBLIC
        ${OpenCV_LIBS}
)

target_compile_definitions(ShiftStitch
    PRIVATE SHIFTSTITCH_BUILD
)

# -------------------------
# Examples
# -------------------------
if(SHIFTSTITCH_BUILD_EXAMPLES)
    add_executable(example_basic examples/basic_example.cpp)
    target_link_libraries(example_basic ShiftStitch)

    if(UNIX AND NOT APPLE)
        set_target_properties(example_basic PROPERTIES
            BUILD_RPATH "$ORIGIN"
            INSTALL_RPATH "$ORIGIN"
        )
    elseif(APPLE)
        set_target_properties(example_basic PROPERTIES
            BUILD_RPATH "@loader_path"
            INSTALL_RPATH "@loader_path"
        )
    endif()
endif()

# -------------------------
# Tests
# -------------------------
if(SHIFTSTITCH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# -------------------------
# Install
# -------------------------
include(GNUInstallDirs)

install(TARGETS ShiftStitch
    EXPORT ShiftStitchTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ShiftStitchTargets
    FILE ShiftStitchTargets.cmake
    NAMESPACE ShiftStitch::
    DESTINATION lib/cmake/ShiftStitch
)