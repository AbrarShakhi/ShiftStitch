function(fetch_opencv)
    include(FetchContent)

    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.x
    )

    # Reduce build size A LOT
    set(BUILD_LIST core,imgproc,features2d,stitching CACHE STRING "" FORCE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_PERF_TESTS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(opencv)

    set(OpenCV_LIBS opencv_core opencv_imgproc opencv_features2d opencv_stitching PARENT_SCOPE)
    set(OpenCV_INCLUDE_DIRS ${opencv_SOURCE_DIR}/include PARENT_SCOPE)
endfunction()