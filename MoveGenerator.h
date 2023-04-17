#ifndef CHESSENGINE_MOVEGENERATOR_H
#define CHESSENGINE_MOVEGENERATOR_H

#include "Move.h"
#include "ChessBoard.h"

class MoveGenerator {
public:
    static std::vector<Moves::Move> pseudoLegalMoves(const ChessBoard& board);
    static bool isSquareAttacked(const ChessBoard& board, short square, Color color);
    static bool inCheck(const ChessBoard& board, Color color);
    static unsigned long long perft(int depth,  ChessBoard& board);
    static bool isLegalMove(ChessBoard &board, Moves::Move move);
};

#endif //CHESSENGINE_MOVEGENERATOR_H
