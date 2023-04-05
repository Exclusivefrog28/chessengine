cmake_minimum_required(VERSION 3.25)
project(ChessEngine)

set(CMAKE_CXX_STANDARD 23)

add_executable(ChessEngine main.cpp ChessBoard.cpp ChessBoard.h Move.h Piece.h)
