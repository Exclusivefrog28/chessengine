#include <iostream>
#include <emscripten/em_macros.h>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"
#include "Util.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "Search.h"

ChessBoard board;

extern "C" {
EMSCRIPTEN_KEEPALIVE
void init() {
    board.hashCodes.initialize();
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
char *getMoves() {
    std::string string;
    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

    bool empty = true;

    string += "{";
    string += R"("moves" : [)";
    for (const Move move: moves) {
        if (MoveGenerator::isLegalMove(board, move)) {
            empty = false;
            string += "{";
            string += R"("start":")";
            string += Util::positionToString(move.start);
            string += R"(","end":")";
            string += Util::positionToString(move.end);
            string += R"(","promotionType":")";
            string += std::to_string(move.promotionType);
            string += R"(","flag":")";
            string += std::to_string(move.flag);
            string += R"(","player":")";
            string += std::to_string(move.player);
            string += "\"},";
        }
    }
    if (!empty) {
        string.pop_back();
    }
    string += R"(],"state":)";

    if (empty) {
        if (MoveGenerator::inCheck(board, board.sideToMove)) string += R"("checkmate")";
        else string += R"("stalemate")";
    } else string += R"("normal")";

    string += "}";

    const int length = string.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, string.c_str());
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
char *getBestMove(int seconds) {
    Move bestMove = Search::search(board, seconds);
    std::string bestMoveJSON = R"({"start":")";
    bestMoveJSON += Util::positionToString(bestMove.start);
    bestMoveJSON += R"(","end":")";
    bestMoveJSON += Util::positionToString(bestMove.end);
    bestMoveJSON += R"(","promotionType":")";
    bestMoveJSON += std::to_string(bestMove.promotionType);
    bestMoveJSON += R"(","flag":")";
    bestMoveJSON += std::to_string(bestMove.flag);
    bestMoveJSON += R"(","player":")";
    bestMoveJSON += std::to_string(bestMove.player);
    bestMoveJSON += "\"}";
    const int length = bestMoveJSON.length();
    char *chararray = new char[length + 1];
    strcpy(chararray, bestMoveJSON.c_str());
    return chararray;
}


EMSCRIPTEN_KEEPALIVE
void setFen(char *fen) {
    std::string fenString(fen);
    board.setPosition(fenString);
}

EMSCRIPTEN_KEEPALIVE
int runPerft(int depth, const char *fen) {
    ChessBoard perftBoard;
    perftBoard.setPosition(fen);

    return MoveGenerator::perft(depth, perftBoard);
}

EMSCRIPTEN_KEEPALIVE
int main() {
    return 0;
}
}