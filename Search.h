#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "Move.h"
#include "ChessBoard.h"

using Moves::Move;

class Search {

public:

    std::array<Move, 3>* killerMoves;
    std::vector<Move> principalVariation;
    ChessBoard&  board;

    Search(int depth, ChessBoard &board);
    virtual ~Search();

    static Moves::Move search(ChessBoard& board, int depth);

private:

    int alphaBeta(int depth, int alpha, int beta, int ply, std::vector<Move>& pv);
    int quiescence(int alpha, int beta, int ply, std::vector<Move>& pv);
    int scoreMove(const Move& move, int ply) const;
    void storeKillerMove(const Move& move, int ply) const;
};


#endif
