cmake_minimum_required(VERSION 3.16)
project(ladxdc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Core library
set(LADX_CORE_SOURCES
    src/gb.c
    src/home/clear_memory.c
    src/home/copy_data.c
    src/home/bank.c
    src/home/lcd.c
    src/home/entities.c
    src/home/audio.c
    src/home/dialog.c
    src/home/room.c
    src/home/gameplay.c
    src/home/link.c
    src/home/vfx.c
    src/home/ui.c
    src/home/gfx.c
)

add_library(ladx_core STATIC ${LADX_CORE_SOURCES})

# Test runner
enable_testing()

add_executable(ladx_tests
    tests/test_runner.c
    tests/test_clear_memory.c
    tests/test_copy_data.c
    tests/test_bank.c
    tests/test_lcd.c
    tests/test_entities.c
    tests/test_audio.c
    tests/test_dialog.c
    tests/test_room.c
    tests/test_gameplay.c
    tests/test_link.c
    tests/test_vfx.c
    tests/test_ui.c
    tests/test_gfx.c
)

target_link_libraries(ladx_tests PRIVATE ladx_core)
add_test(NAME ladx_unit_tests COMMAND ladx_tests)
