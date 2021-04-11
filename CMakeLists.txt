cmake_minimum_required(VERSION 3.0)
project(print-c-connector VERSION 1.0 LANGUAGES C)

add_executable(print_space_ex ../src/print_space.c ../src/mp_source.c ../src/msgpuck_print.c)
target_link_libraries(print_space_ex PUBLIC tarantool)
