cmake_minimum_required(VERSION 3.0)
project(print-c-connector VERSION 1.0 LANGUAGES C)

add_executable(print_space ./src/print_space.c)
target_link_libraries(print_space PUBLIC tarantool)


