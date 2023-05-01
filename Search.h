#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "Move.h"
#include "ChessBoard.h"

using Moves::Move;

class Search {

public:
    static Moves::Move search(ChessBoard& board, int depth);

private:
    static int alphaBeta(ChessBoard& board, int depth, int alpha, int beta);
};


#endif
