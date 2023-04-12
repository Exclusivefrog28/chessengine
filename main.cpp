#include <iostream>
#include <emscripten/em_macros.h>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"
#include "Util.h"

ChessBoard board;

extern "C" {
EMSCRIPTEN_KEEPALIVE
void init() {
    board.setStartingPosition();
}
EMSCRIPTEN_KEEPALIVE
char* move(int start, int end, int flag, int promotionType, int player) {
    board.makeMove({static_cast<short>(start), static_cast<short>(end), static_cast<Piece::Type>(promotionType),
                    static_cast<MoveFlag>(flag), static_cast<Piece::Color>(player)});

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
char* listPieces(){
    std::string string = "White pieces: ";
    for (ChessBoard::Piece &piece: board.whitePieces){
        string += "[";
        string += Util::pieceToString(piece.type, Piece::WHITE);
        string += ", ";
        string += std::to_string(piece.position);
        string += "] ";
    }
    string += "\nBlack pieces:";
    for (ChessBoard::Piece &piece: board.blackPieces){
        string += "[";
        string += Util::pieceToString(piece.type, Piece::BLACK);
        string += ", ";
        string += std::to_string(piece.position);
        string += "] ";
    }
    string += "\n";

    const int length = string.length();
    char* chararray = new char[length + 1];
    strcpy(chararray, string.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
int main() {


    board.setStartingPosition();

    std::cout << board.fen() << std::endl;

    return 0;
}
}