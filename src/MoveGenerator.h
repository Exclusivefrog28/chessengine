#ifndef CHESSENGINE_MOVEGENERATOR_H
#define CHESSENGINE_MOVEGENERATOR_H

#include "Move.h"
#include "ChessBoard.h"

class MoveGenerator {
public:
    static std::vector<Move> pseudoLegalMoves(const ChessBoard& board);
    static std::vector<Move> tacticalMoves(const ChessBoard& board);
    static bool isSquareAttacked(const ChessBoard& board, int_fast8_t square, Color color);
    static bool inCheck(const ChessBoard& board, Color color);
    static unsigned long long perft(int depth,  ChessBoard& board);
    static bool isLegalMove(ChessBoard &board, Move move);
};

#endif //CHESSENGINE_MOVEGENERATOR_H
