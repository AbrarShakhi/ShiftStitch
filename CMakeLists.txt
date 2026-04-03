cmake_minimum_required(VERSION 3.16)

project(ShiftStitch
    VERSION 1.0.0
    DESCRIPTION "Panorama stitching library using shift algorithm"
    LANGUAGES CXX
)

# Options
option(SHIFTSTITCH_BUILD_TESTS "Build tests" ON)
option(SHIFTSTITCH_BUILD_EXAMPLES "Build examples" ON)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Position independent code (important for shared libs / Android)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---- OpenCV ----
find_package(OpenCV QUIET)

if(NOT OpenCV_FOUND)
    message(STATUS "OpenCV not found, fetching...")

    include(FetchContent)

    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.x
    )

    # Optional: reduce build size
    set(BUILD_LIST core,imgproc,features2d,stitching CACHE STRING "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(opencv)

    set(OpenCV_LIBS opencv_core opencv_imgproc opencv_features2d opencv_stitching)
    set(OpenCV_INCLUDE_DIRS ${opencv_SOURCE_DIR}/include)
endif()

# ---- Library ----
add_library(ShiftStitch SHARED
    src/shiftstitch.cpp
    src/panorama.cpp
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

# ---- Install ----
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

# ---- Tests ----
if(SHIFTSTITCH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---- Examples ----
if(SHIFTSTITCH_BUILD_EXAMPLES)
    add_executable(example_basic examples/basic_example.cpp)
    target_link_libraries(example_basic ShiftStitch)
endif()