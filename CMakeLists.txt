cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

add_executable(ChessEngine main.cpp ChessBoard.cpp ChessBoard.h Move.h Piece.h Util.h MoveGenerator.cpp MoveGenerator.h)

set_target_properties(ChessEngine PROPERTIES
        LINK_FLAGS "-s WASM=1 -s ENVIRONMENT=web -s EXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces','_getMoves'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        )