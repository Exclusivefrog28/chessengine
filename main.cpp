#include <iostream>
#include <emscripten/em_macros.h>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"
#include "Util.h"
#include "MoveGenerator.h"
#include "Evaluator.h"

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
int eval() {
    return Evaluator::evaluate(board);
}

EMSCRIPTEN_KEEPALIVE
void setFen(char *fen) {
    std::string fenString(fen);
    board.setPosition(fenString);
}

EMSCRIPTEN_KEEPALIVE
int runPerft(int depth, const char* fen) {
    ChessBoard perftBoard;
    perftBoard.setPosition(fen);

    return MoveGenerator::perft(depth, perftBoard);
}

EMSCRIPTEN_KEEPALIVE
int main() {

//    board.setPosition("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
//
//    std::cout << board.fen() << std::endl;
//
//    for (int i = 0; i < 11; ++i) {
//        std::chrono::time_point start = std::chrono::high_resolution_clock::now();
//        unsigned long long nodes =  MoveGenerator::perft(i, board);
//        std::cout << i << " : " << nodes << " nodes in " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count() << "ms\n" << std::endl;
//    }
//    std::cout << board.fen() << std::endl;

    return 0;
}
}