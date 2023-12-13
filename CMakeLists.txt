cmake_minimum_required(VERSION 3.26)
project(storage_manager C)

set(CMAKE_C_STANDARD 11)

add_executable(storage_manager main.c
        dir_tree/dir_tree.c
        dir_tree/dir_tree.h
        dir_tree/mk.c
        dir_tree/internal.h
        shared/fio.h
        shared/fio.c
        luigi/luigi.h
        ref_logic.def
        unref_logic.def
)

target_link_libraries(storage_manager user32.lib gdi32.lib)
