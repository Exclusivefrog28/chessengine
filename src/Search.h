#ifndef CHESSENGINE_SEARCH_H
#define CHESSENGINE_SEARCH_H

#include "Move.h"
#include "ChessBoard.h"
#include "TranspositionTable.h"
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstdint>

#include "Logger.h"

using namespace Moves;

struct ScoredMove {
	Move move;
	int score;
};

class Search {
public:
	ChessBoard&board;
	static TranspositionTable tt;

	explicit Search(ChessBoard&board);

    void doSearch();

    Move endSearch(int timeOut);

    void reset();

private:
	std::array<std::array<Move, 2>, 64> killerMoves{};
	bool killerMoveIndexOne = false;
	std::array<std::array<std::array<int, 64>, 64>, 2> history{};
	std::vector<Move> lastPV;

	bool stop = false;

    std::binary_semaphore searchingSemaphore{1};
    std::binary_semaphore reachedDepthOneSemaphore{1};

	Logger logger;

	void threadedSearch();

	int alphaBeta(int depth, int alpha, int beta, int ply);

	int quiesce(int alpha, int beta, int ply, int depth);

	std::vector<ScoredMove> scoreMoves(const std::vector<Move>&moves, int ply, const Move&hashMove) const;

	std::vector<ScoredMove> scoreTacticalMoves(const std::vector<Move>&moves, const Move&hashMove) const;

	static bool getTransposition(uint64_t hash, int depth, int ply, int&score, const int&alpha, const int&beta, Move&hashMove);

	static Move selectMove(std::vector<ScoredMove>&moves, int index);

	void storeKillerMove(const Move&move, int ply);

    std::vector<Move> collectPV(int depth) const;
	std::vector<Move> collectPV(int depth, bool& endEarly) const;
};

#endif
