cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)
option(WASM "Build Webassembly binary" TRUE)
option(LOGTOFILE "Make a file with a detailed search log" FALSE)

add_executable(ChessEngine main.cpp)

set(CMAKE_CXX_FLAGS "-std=c++23")

if (WASM)
    message("Building for Webassembly")
    add_compile_definitions(wasm)
    set(CMAKE_CXX_FLAGS "-std=c++23 -fexperimental-library")
endif ()

if (LOGTOFILE)
    message("Logging to file enabled")
    add_compile_definitions(logtofile)
endif ()

include_directories(src)
add_subdirectory(src)

target_link_libraries(ChessEngine src)

if (NOT WASM)
    add_subdirectory(tests)
endif ()

if (WASM)
    set_target_properties(src PROPERTIES
            COMPILE_FLAGS "-pthread"
    )
    set_target_properties(ChessEngine PROPERTIES
            LINK_FLAGS "-O3 --closure 1 -sMODULARIZE -sEXPORT_ES6=1 -sEXPORT_NAME=Engine -sENVIRONMENT=web,worker -pthread -sPTHREAD_POOL_SIZE=8 --embed-file ../assets@/ -sINITIAL_MEMORY=512MB -sEXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces','_getMoves','_getAttacks','_setFen','_runPerft','_eval','_startSearch','_stopSearch'] -sEXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
            COMPILE_FLAGS "-pthread"
    )
    #-s ENVIRONMENT=web / -s ENVIRONMENT=node
    #--profiling -g -s ASSERTIONS
endif ()

if (NOT WASM)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()