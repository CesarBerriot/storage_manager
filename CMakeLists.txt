cmake_minimum_required(VERSION 3.26)
project(storage_manager C)

set(CMAKE_C_STANDARD 11)

add_executable(storage_manager WIN32
        main.c
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
        # ensure easy portability
        --entry=main
        # hardware acceleration
        -fno-openacc
        # other stuff
        -pthread
)

target_compile_options(storage_manager PRIVATE
        # ensure easy portability
        -fno-asm
        -fno-builtin
        -fno-plan9-extensions
        -fsigned-bitfields
        -fsigned-char
        -fmax-include-depth=17 # keeping things organized. that's the lowest I could get without breaking mingw.
        -fno-extended-identifiers
        -fexec-charset=utf-8
        -fshort-wchar
        -fwide-exec-charset=utf-16be
        -finput-charset=utf-8
        # hardware acceleration
        # turns out I had problems getting some performance out of openacc so I ended up
        # just multithreading the whole thing myself using pthread and it works wonders
        # I didn't try openmp yet, I might give it a try later, might even use opencl
        -fno-openacc
        -fno-openmp
        # unrelated gcc optimization/code generation options
        -O3
        -fstack-reuse=all
        -freg-struct-return # I never return large structure so this option's a plus. in case I gotta get large amounts of information out of a function I'll either take in a pointer to a struct I'll write into or return a pointer to a struct I allocated.
        -fno-exceptions
        -fno-common # prevents tentative definitions (treating uninitialized multiple definitions as an extern definition defined in a single translation unit)
        -fshort-enums
        # global preprocessor definitions
        -DUI_LOADING_SCREEN_LABELS_STR_BUFFERS_LENGTH=100
        -DLOGIC_INFINITE_ANALYSIS=1 # make analysis of the file architecture infinite (basically adds for(;;)) for debugging purposes
        -DLOGIC_ANALYSIS_MAX_THREADING_DEPTH=5
        # silencing trivial warnings
        -Wno-incompatible-pointer-types
        -Wno-compare-distinct-pointer-types # broken on gcc ? check out RENDER_LIST() is rendering.c
        # other stuff
        -pthread
        # -flarge-source-files # enable if large headers get included into the program, like files bundled into arrays or whatever
        # -o C:/storage_manager.exe
)

set(RUNTIME_OUTPUT_DIRECTORY C:/)
