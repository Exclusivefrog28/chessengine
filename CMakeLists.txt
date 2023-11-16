cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

option(WASM "Build Webassembly binary" OFF)
option(EXEC "Build executable" ON)

if(WASM)
    add_compile_definitions(wasm)
    set_target_properties(ChessEngine PROPERTIES
            LINK_FLAGS "-s WASM=1 -O3 -s ENVIRONMENT=web,worker -pthread -s INITIAL_MEMORY=512MB -s STACK_SIZE=256MB -s ALLOW_MEMORY_GROWTH -s EXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces','_getMoves','_getAttacks','_setFen','_runPerft','_eval','_getBestMove'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
            BUILD_FLAGS ""
    )
endif()

add_executable(ChessEngine main.cpp ChessBoard.cpp ChessBoard.h Move.h Piece.h Util.h MoveGenerator.cpp MoveGenerator.h Evaluator.cpp Evaluator.h EvaluationValues.h Search.cpp Search.h HashCodes.h TranspositionTable.cpp TranspositionTable.h)


#-s ENVIRONMENT=web / -s ENVIRONMENT=node
#--profiling -g -s ASSERTIONS