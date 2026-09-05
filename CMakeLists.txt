cmake_minimum_required(VERSION 3.15)
project(ladx_decomp C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-unused-parameter")

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
)

target_link_libraries(ladx_tests PRIVATE ladx_core)
add_test(NAME ladx_unit_tests COMMAND ladx_tests)
