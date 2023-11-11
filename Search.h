#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "Move.h"
#include "ChessBoard.h"
#include "TranspositionTable.h"
#include <array>

using namespace Moves;

struct ScoredMove {
    Move move;
    int score;
};

class Search {

public:
    ChessBoard&  board;

    explicit Search(ChessBoard &board);

    static Move search(ChessBoard& board, int timeOut);

private:

    std::array<std::array<Move, 2>,64> killerMoves{};
    bool killerMoveIndexOne = false;
    std::array<std::array<std::array<int, 64>,64>,2> history{};
    std::vector<Move> lastPV;

    static TranspositionTable tt;

    int alphaBeta(int depth, int alpha, int beta, int ply);
    int quiesce(int alpha, int beta, int ply, int depth);
    std::vector<ScoredMove> scoreMoves(const std::vector<Move>& moves, int ply, Move hashMove) const;
    std::vector<ScoredMove> scoreTacticalMoves(const std::vector<Move>& moves, Move hashMove) const;
    static bool getTransposition(unsigned long int hash, int depth, int ply, int& score, int& alpha, int& beta, Move& hashMove) ;
    static Move selectMove(std::vector<ScoredMove>& moves, int index);
    void storeKillerMove(Move move, int ply);
    std::vector<Move> collectPV(int depth, bool& gameOver) const;
};

#endif
