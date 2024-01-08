cmake_minimum_required(VERSION 3.26)
project(storage_manager C)

set(CMAKE_C_STANDARD 11)

add_executable(storage_manager main.c
        logic/dir_tree/dir_tree.c
        logic/dir_tree/dir_tree.h
        logic/dir_tree/mk.c
        logic/dir_tree/internal.h
        shared/fio.h
        shared/fio.c
        luigi/luigi.h
        logic/ref_globals.def
        logic/unref_globals.def
        ui/rendering.h
        ui/rendering.c
        ui/callbacks.c
        ui/callbacks.h
        ui/ui.h
        logic/logic.h
        logic/logic.c
        shared/macros.h
        ui/ui.c)

target_link_libraries(storage_manager user32.lib gdi32.lib)

target_link_options(storage_manager PRIVATE
        -fno-openacc
)

target_compile_options(storage_manager PRIVATE
        # ensure easy portability
        -fno-asm
        -fno-builtin
        # hardware acceleration
        -fno-openacc
        # silencing trivial warnings
        -Wno-incompatible-pointer-types
        -Wno-compare-distinct-pointer-types # broken on gcc ? check out RENDER_LIST() is rendering.c
)
