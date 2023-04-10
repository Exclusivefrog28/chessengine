#include <iostream>
#include <emscripten/em_macros.h>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"

ChessBoard board;

extern "C" {
EMSCRIPTEN_KEEPALIVE
void init() {
    board.setStartingPosition();
}
EMSCRIPTEN_KEEPALIVE
char* move(int start, int end, int flag, int promotionType, int player) {
    board.makeMove({static_cast<short>(start), static_cast<short>(end), static_cast<Type>(promotionType),
                    static_cast<MoveFlag>(flag), static_cast<Color>(player)});

    std::string fen = board.fen();
    const int length = fen.length();
    char* chararray = new char[length + 1];
    strcpy(chararray, fen.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
char* unmove(int start, int end) {
    board.unMakeMove();

    std::string fen = board.fen();
    const int length = fen.length();
    char* chararray = new char[length + 1];
    strcpy(chararray, fen.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
int main() {


    board.setStartingPosition();

    std::cout << board.fen() << std::endl;
    board.makeMove({7 * 8 + 1, 5 * 8 + 2, EMPTY, QUIET, WHITE});
    std::cout << board.fen() << std::endl;
    board.unMakeMove();
    std::cout << board.fen() << std::endl;

    return 0;
}
}