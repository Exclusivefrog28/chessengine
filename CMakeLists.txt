cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

add_executable(ChessEngine main.cpp ChessBoard.cpp ChessBoard.h Move.h Piece.h)

set_target_properties(ChessEngine PROPERTIES
        LINK_FLAGS "-s WASM=1 -s ENVIRONMENT=node -s EXPORTED_FUNCTIONS=['_main','_init','_move','_unmove','_listPieces'] -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
        )