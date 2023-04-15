#ifndef CHESSENGINE_MOVEGENERATOR_H
#define CHESSENGINE_MOVEGENERATOR_H

#include "Move.h"
#include "ChessBoard.h"

class MoveGenerator {
public:
    static std::vector<Moves::Move> pseudoLegalMoves(ChessBoard board);
    static bool isSquareAttacked(const ChessBoard& board, short square, Color color);

};

#endif //CHESSENGINE_MOVEGENERATOR_H
