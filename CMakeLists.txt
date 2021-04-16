cmake_minimum_required(VERSION 3.0)
project(print-c-connector VERSION 1.0 LANGUAGES C CXX)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


add_executable(print_space_ex ../src/print_space.c ../src/mp_source.c ../src/msgpuck_print.c ../src/msgpuck_sprint.c ../src/improved_select.c ../src/additional_utilities.c)
#target_sources(print_space_ex ../src/improved_select.c)
#target_sources(print_space_ex ../src/additional_utilities.c)
target_link_libraries(print_space_ex PUBLIC tarantool)


add_executable(PrintMsgPuckTest.test test/PrintMsgPuckTest.cpp src/msgpuck_sprint.c src/mp_source.c)

enable_testing()
add_test(NAME PrintMsgPuckTest.test COMMAND PrintMsgPuckTest.test)