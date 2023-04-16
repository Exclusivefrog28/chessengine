#include <iostream>
#include <emscripten/em_macros.h>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"
#include "Util.h"
#include "MoveGenerator.h"

ChessBoard board;

extern "C" {
EMSCRIPTEN_KEEPALIVE
void init() {
    board.setStartingPosition();
}
EMSCRIPTEN_KEEPALIVE
char *move(int start, int end, int flag, int promotionType, int player) {
    board.makeMove({static_cast<short>(start), static_cast<short>(end), static_cast<Pieces::Type>(promotionType),
                    static_cast<MoveFlag>(flag), static_cast<Pieces::Color>(player)});

    std::string fen = board.fen();
    const int length = fen.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, fen.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
char *unmove() {
    board.unMakeMove();

    std::string fen = board.fen();
    const int length = fen.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, fen.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
char *listPieces() {
    std::string string = "White pieces: ";
    for (Piece &piece: board.whitePieces) {
        string += "[";
        string += Util::pieceToString(piece.type, Pieces::WHITE);
        string += ", ";
        string += std::to_string(piece.position);
        string += "] ";
    }
    string += "\nBlack pieces:";
    for (Piece &piece: board.blackPieces) {
        string += "[";
        string += Util::pieceToString(piece.type, Pieces::BLACK);
        string += ", ";
        string += std::to_string(piece.position);
        string += "] ";
    }
    string += "\n";

    const int length = string.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, string.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
char *getMoves(int position) {
    std::string selectedMoves;
    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

    bool empty = true;

    selectedMoves += "[";
    for (const Move move: moves) {
        if (move.start == position && MoveGenerator::isLegalMove(board, move)) {
            empty = false;
            selectedMoves += "{";
            selectedMoves += R"("start":")";
            selectedMoves += Util::positionToString(move.start);
            selectedMoves += R"(","end":")";
            selectedMoves += Util::positionToString(move.end);
            selectedMoves += R"(","promotionType":")";
            selectedMoves += std::to_string(move.promotionType);
            selectedMoves += R"(","flag":")";
            selectedMoves += std::to_string(move.flag);
            selectedMoves += R"(","player":")";
            selectedMoves += std::to_string(move.player);
            selectedMoves += "\"},";
        }
    }
    if (!empty) selectedMoves.pop_back();
    selectedMoves += "]";

    const int length = selectedMoves.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, selectedMoves.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
char *getAttacks() {
    std::string attackedSquares;
    for (short i = 0; i < 64; ++i) {
        if (MoveGenerator::isSquareAttacked(board, i, board.sideToMove)) {
            attackedSquares += Util::positionToString(i);
            attackedSquares += " ";
        }
    }
    if (!attackedSquares.empty()) attackedSquares.pop_back();

    const int length = attackedSquares.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, attackedSquares.c_str());
    return chararray;
}

EMSCRIPTEN_KEEPALIVE
void setFen(char *fen) {
    std::string fenString(fen);
    board.setPosition(fenString);
}

EMSCRIPTEN_KEEPALIVE
int main() {
//    board.setStartingPosition();
//
//    std::cout << board.fen() << std::endl;
//
//    board.setPosition("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
//
//    std::cout << board.fen() << std::endl;
//
//    printf("%s", listPieces());
//
//    for (int i = 0; i < 7; ++i) {
//        std::cout << i << " : " << MoveGenerator::perft(i, board) << "\n";
//    }
//    std::cout << board.fen() << std::endl;
//
//    printf("%s", listPieces());

    return 0;
}
}