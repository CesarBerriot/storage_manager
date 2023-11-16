cmake_minimum_required(VERSION 3.26)
project(storage_manager C)

set(CMAKE_C_STANDARD 11)

add_executable(storage_manager main.c
        dir_tree/dir_tree.c
        dir_tree/dir_tree.h
        dir_tree/mk.c
        dir_tree/internal.h
        shared/cwd.h
        shared/cwd.c
)
