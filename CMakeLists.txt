cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

option(WASM "Build Webassembly binary" TRUE)

add_executable(ChessEngine main.cpp)

include_directories(src)
add_subdirectory(src)

target_link_libraries(ChessEngine src)

add_subdirectory(tests)

message("WASM=${WASM}")
if (WASM)
    message("Building for Webassembly")
    add_compile_definitions(wasm)
    set_target_properties(ChessEngine PROPERTIES
            LINK_FLAGS "-s WASM=1 -O3 -s ENVIRONMENT=web,worker -pthread -s PTHREAD_POOL_SIZE=8 -s INITIAL_MEMORY=600MB -s ALLOW_MEMORY_GROWTH -s EXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces','_getMoves','_getAttacks','_setFen','_runPerft','_eval','_getBestMove'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
            COMPILE_FLAGS "-pthread"
    )
    #-s ENVIRONMENT=web / -s ENVIRONMENT=node
    #--profiling -g -s ASSERTIONS
endif ()

if (NOT WASM)
    set(CMAKE_EXE_LINKER_FLAGS "-static")
endif ()