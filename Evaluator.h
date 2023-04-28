#ifndef CHESSENGINE_EVALUATOR_H
#define CHESSENGINE_EVALUATOR_H

#include "ChessBoard.h"

class Evaluator {

    enum MaterialValues {
        PAWN_VALUE = 100,
        KNIGHT_VALUE = 350,
        BISHOP_VALUE = 350,
        ROOK_VALUE = 525,
        QUEEN_VALUE = 1000,
    };


public:
    static int evaluate(const ChessBoard &board);

private:

    constexpr static std::array<int,5> materialValues = {100, 350, 350, 525, 1000};

    static int materialScore(const ChessBoard &board, Color side);
};

#endif //CHESSENGINE_EVALUATOR_H
