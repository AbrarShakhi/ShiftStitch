add_executable(ShiftStitchTests test_main.cpp)

target_link_libraries(ShiftStitchTests
    PRIVATE ShiftStitch
)

add_test(NAME ShiftStitchTests COMMAND ShiftStitchTests)