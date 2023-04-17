cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

add_executable(ChessEngine main.cpp ChessBoard.cpp ChessBoard.h Move.h Piece.h Util.h MoveGenerator.cpp MoveGenerator.h)

set_target_properties(ChessEngine PROPERTIES
        LINK_FLAGS "-s WASM=1 -O3 --profiling -s ENVIRONMENT=web -s ALLOW_MEMORY_GROWTH -s EXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces','_getMoves','_getAttacks','_setFen','_runPerft'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        )