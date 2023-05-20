#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "Move.h"
#include "ChessBoard.h"
#include <array>

using namespace Moves;

struct ScoredMove {
    Move move;
    int score;
};

class Search {

public:
    std::vector<Move> principalVariation;
    ChessBoard&  board;

    static Move search(ChessBoard& board, int depth);

    Search(ChessBoard &board, int depth);
    virtual ~Search();

private:

    std::array<Move, 2>* killerMoves;
    bool killerMoveIndexOne = false;
    std::array<std::array<std::array<int, 64>,64>,2> history;
    std::vector<Move> lastPV;

    int alphaBeta(int depth, int alpha, int beta, int ply, std::vector<Move>& pv);
    int quiesce(int alpha, int beta, int ply, std::vector<Move>& pv);
    std::vector<ScoredMove> scoreMoves(const std::vector<Move>& moves, int ply, Move* hashMove) const;
    std::vector<ScoredMove> scoreTacticalMoves(const std::vector<Move>& moves) const;
    Move selectMove(std::vector<ScoredMove>& moves, int index) const;
    void storeKillerMove(Move move, int ply);
};


#endif
