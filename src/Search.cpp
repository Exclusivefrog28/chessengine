#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"
#include <unordered_set>
#include <chrono>
#include <string>
#include <thread>

#define MATE_SCORE 65536

TranspositionTable Search::tt = TranspositionTable();

Move Search::search(ChessBoard&board, const int timeAllowed) {
	Search search = Search(board);

	const auto timeOut = std::chrono::milliseconds(timeAllowed);

	const auto start = std::chrono::steady_clock::now();

	int i = 1;
	for (; i < 100; ++i) {
		std::thread thread(&Search::threadedSearch, &search, i);

		std::unique_lock<std::mutex> lk(search.cv_m);
		search.stop = false;
		search.finished = false;

		const auto timeAvailable = start + timeOut - std::chrono::steady_clock::now();

		if (search.cv.wait_for(lk, timeAvailable, [&] { return search.finished; })) {
			thread.join();
		}
		else {
			search.stop = true;
			thread.join();
			break;
		}

		//if (abs(tt.getEntry(board.hashCode, 0).score) == MATE_SCORE) break;
	}
#ifdef wasm
	printf("Depth: %d\n", i - 1);
	int score = Evaluator::evaluate(board);
	if (tt.contains(board.hashCode)) {
		const TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
		score = entry.score;
	}
	printf("Evaluation: %d\nPV: ", score);
	for (const Move&move: search.lastPV) {
		printf("%s%s ", Util::positionToString(move.start).c_str(), Util::positionToString(move.end).c_str());
	}
	printf("\nBoard hash: %llu", board.hashCode);
	printf("\nTT reads: %d", tt.reads);
	printf("\nTT writes: %d", tt.writes);
	printf("\nTT collisions: %d", tt.collisions);
	printf("\nTT occupancy: %d", tt.occupancy());
	printf("\n**************************\n");
#endif
	search.lastPV = search.collectPV(i);

	tt.resetCounters();

	if (search.lastPV.empty()) {
		auto entry = tt.getEntry(board.hashCode, 0);
		std::cout << "problem HASH: " << board.hashCode << " TT: key- " << entry.key << ", move- " << entry.bestMove << ", type- " << entry.nodeType <<
				", depth- " << entry.depth << ", score- " << entry.score << std::endl;
		return NULL_MOVE;
	}
	return search.lastPV[0];
}

void Search::threadedSearch(int depth) {
	constexpr int alpha = INT32_MIN + 1;
	constexpr int beta = INT32_MAX;

	alphaBeta(depth, alpha, beta, 0);
	if (stop) return;

	std::lock_guard<std::mutex> lk(cv_m);
	finished = true;

	cv.notify_one();
}

int Search::alphaBeta(const int depth, int alpha, int beta, const int ply) {
	if (stop) { return 0; }
	if (depth == 0) return quiesce(alpha, beta, ply, 0);

	Move hashMove = NULL_MOVE;
	int positionScore = 0;
	if (getTransposition(board.hashCode, depth, ply, positionScore, alpha, beta, hashMove)) return positionScore;

	alpha = std::max(alpha, -MATE_SCORE + ply);
	beta = std::min(beta, MATE_SCORE - ply);
	if (alpha >= beta) return alpha;

	std::vector<ScoredMove> moves = scoreMoves(MoveGenerator::pseudoLegalMoves(board), ply, hashMove);

	bool hasLegalMoves = false;

	TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
	Move bestMove = NULL_MOVE;
	int bestScore = INT32_MIN;

	for (int i = 0; i < moves.size(); i++) {
		Move move = selectMove(moves, i);
		board.makeMove(move);
		if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
			board.unMakeMove();
			continue;
		}

		hasLegalMoves = true;

		//50 move rule
		bool draw = board.halfMoveClock >= 100 &&
		            !(board.squares[move.end].type == PAWN || move.flag >= 1 || move.promotionType != EMPTY);

		//repetition
		if (!draw) draw = board.isRepetition();

		const int score = draw ? 0 : -alphaBeta(depth - 1, -beta, -alpha, ply + 1);
		board.unMakeMove();

		if (stop) return 0;

		if (score >= beta) {
			if (move.flag == 0 || move.flag >= 7) {
				storeKillerMove(move, ply);
				history[board.sideToMove][move.start][move.end] += depth * depth;
			}

			tt.setEntry(board, move, depth, score, TranspositionTable::LOWERBOUND, ply);

			return score;
		}
		if (score > alpha) {
			alpha = score;
			bestScore = score;
			bestMove = move;
			nodeType = TranspositionTable::EXACT;
		}
		else if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
	}
	if (!hasLegalMoves) {
		if (MoveGenerator::inCheck(board, board.sideToMove)) return -(MATE_SCORE - ply);
		return 0;
	}

	tt.setEntry(board, bestMove, depth, bestScore, nodeType, ply);

	return alpha;
}

int Search::quiesce(int alpha, int beta, const int ply, const int depth) {
	if (stop) return 0;
	const int stand_pat = Evaluator::evaluate(board);
	if (stand_pat >= beta)
		return beta;
	if (alpha < stand_pat)
		alpha = stand_pat;
	if (stand_pat + mg_value[QUEEN - 1] < alpha) {
		return alpha;
	}

	Move hashMove = NULL_MOVE;
	int positionScore = 0;
	if (getTransposition(board.hashCode, depth, ply, positionScore, alpha, beta, hashMove)) return positionScore;

	std::vector<ScoredMove> moves = scoreTacticalMoves(MoveGenerator::tacticalMoves(board), hashMove);

	TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
	Move bestMove = {-1, -1, EMPTY, QUIET, WHITE};
	int bestScore = INT32_MIN;

	for (int i = 0; i < moves.size(); i++) {
		Move move = selectMove(moves, i);


		board.makeMove(move);
		if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
			board.unMakeMove();
			continue;
		}
		const bool draw = board.isRepetition();

		const int score = draw ? 0 : -quiesce(-beta, -alpha, ply + 1, depth - 1);
		board.unMakeMove();

		if (stop) return 0;

		if (score >= beta) {
			tt.setEntry(board, move, depth, score, TranspositionTable::LOWERBOUND, ply);
			return score;
		}
		if (score > alpha) {
			alpha = score;
			bestScore = score;
			bestMove = move;
			nodeType = TranspositionTable::EXACT;
		}
		else if (score > bestScore) {
			bestScore = score;
			bestMove = move;
		}
	}
	tt.setEntry(board, bestMove, depth, bestScore, nodeType, ply);

	return alpha;
}

std::vector<ScoredMove> Search::scoreMoves(const std::vector<Move>&moves, const int ply, const Move&hashMove) const {
	std::vector<ScoredMove> scoredMoves;
	for (const Move&move: moves) {
		int score = 0;

		if (move == hashMove) score = 1 << 30;

		else if (move.promotionType != 0) {
			score = mg_value[move.promotionType - 1] - mg_value[0];
		}
		else if (move.flag == 0 || move.flag >= 7) {
			if (move == killerMoves[ply][0] || move == killerMoves[ply][1])
				score = 1 << 14;
			else score = history[board.sideToMove][move.start][move.end];
		}
		else {
			if (move.flag == 6) score = 1 << 16;
			else {
				int captureScore = 0;
				const int agressor = mg_value[board.squares[move.start].type - 1];
				const int victim = mg_value[move.flag - 1];
				captureScore += victim - agressor;
				if (captureScore == 0) captureScore = 1;
				if (captureScore > 0) captureScore <<= 16;
				score += captureScore;
			}
		}

		scoredMoves.push_back({move, score});
	}
	return scoredMoves;
}

std::vector<ScoredMove> Search::scoreTacticalMoves(const std::vector<Move>&moves, const Move&hashMove) const {
	std::vector<ScoredMove> scoredMoves;

	for (const Move&move: moves) {
		int score;

		if (move == hashMove) score = 1 << 30;
		else {
			if (move.promotionType != 0)
				score = mg_value[move.promotionType - 1] - mg_value[0];
			else
				score = mg_value[move.flag - 1] -
				        mg_value[board.squares[move.start].type - 1];
		}

		scoredMoves.push_back({move, score});
	}
	return scoredMoves;
}

Move Search::selectMove(std::vector<ScoredMove>&moves, const int index) {
	int selectedIndex = index;
	ScoredMove selected = moves[selectedIndex];
	int maxScore = selected.score;

	for (int i = index + 1; i < moves.size(); ++i) {
		if (moves[i].score > maxScore) {
			selectedIndex = i;
			selected = moves[selectedIndex];
			maxScore = selected.score;
		}
	}
	moves[selectedIndex] = moves[index];
	moves[index] = selected;

	return selected.move;
}

void Search::storeKillerMove(Move move, int ply) {
	if ((move.flag == 0 || move.flag >= 7) && move.promotionType == 0) {
		if (killerMoves[ply][0] == move) return;
		if (killerMoves[ply][1] == move) return;

		if (killerMoveIndexOne || killerMoves[ply][0] == NULL_MOVE) {
			killerMoves[ply][0] = move;
			killerMoveIndexOne = false;
		}
		else {
			killerMoves[ply][1] = move;
			killerMoveIndexOne = true;
		}
	}
}

Search::Search(ChessBoard&board) : board(board) {
}

std::vector<Move> Search::collectPV(const int depth) const {
	std::vector<Move> pv;
	std::unordered_set<uint64_t> pvPositions;
	std::vector<int> scores;
	pv.reserve(depth);

	int pvDepth = 0;
	while (tt.contains(board.hashCode)) {
		const TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
		if (entry.nodeType != TranspositionTable::EXACT) break;
		Move move = entry.bestMove;

		if (pvPositions.contains(board.hashCode)) {
			std::cout << "Cycle in PV!" << std::endl;
			break;
		}
#ifdef DEBUG
		auto moves = MoveGenerator::pseudoLegalMoves(board);
		if (moves.empty()) break;
		bool legal = false;
		for (const Move&m: moves) {
			if (m == move) {
				legal = true;
				break;
			}
		}
		if (legal) legal = MoveGenerator::isLegalMove(board, move);
		if (!legal) {
			std::cout << "Illegal move in PV! - " << Util::positionToString(move.start) <<
					Util::positionToString(move.end) << " " << move.flag << " Entry depth: " << entry.depth << " FEN: " << board.fen() << " Hashes: " << board.
					hashCode << std::endl;
			break;
		}
#endif

		pvPositions.insert(board.hashCode);
		board.makeMove(move);
		pv.push_back(move);
		scores.push_back(entry.score);
		pvDepth++;
	}
	std::cout << "info PV: ";
	for (int i = 0; i < pv.size(); ++i) {
		std::cout << "[" << pv[i] << " - " << scores[i] << "] ";
	}
	std::cout << std::endl;

	for (; pvDepth > 0; --pvDepth) {
		board.unMakeMove();
	}

	return pv;
}

bool Search::getTransposition(const uint64_t hash, const int depth, const int ply, int&score, int&alpha, int&beta, Move&hashMove) {
	if (tt.contains(hash)) {
		const TranspositionTable::Entry entry = tt.getEntry(hash, ply);
		if (entry.depth >= depth) {
			switch (entry.nodeType) {
				case TranspositionTable::EXACT:
					score = entry.score;
					return true;
				case TranspositionTable::UPPERBOUND:
					if (entry.score <= alpha) {
						score = entry.score;
						return true;
					}
					break;
				case TranspositionTable::LOWERBOUND:
					if (entry.score >= beta) {
						score = entry.score;
						return true;
					}
				default: break;
			}
		}
		hashMove = entry.bestMove;
	}

	return false;
}
