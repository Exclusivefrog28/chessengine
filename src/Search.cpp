#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"
#include <unordered_set>
#include <chrono>
#include <thread>
#include <format>

#define MATE_SCORE INT32_MAX

TranspositionTable Search::tt = TranspositionTable();

void Search::doSearch() {
    searchingSemaphore.acquire();

    logger.start();
    logger.logToFile("starting search\n");

    reachedDepthOneSemaphore.acquire();
    std::thread thread(&Search::threadedSearch, this);
    thread.detach();
}

Move Search::endSearch(int timeOut) {
    if (timeOut == 0) {
        reachedDepthOneSemaphore.acquire();
        stop = true;
        searchingSemaphore.acquire();

    } else {
        if (!searchingSemaphore.try_acquire_for(std::chrono::milliseconds(timeOut))) {
            reachedDepthOneSemaphore.acquire();
            stop = true;
            searchingSemaphore.acquire();
        }
    }
    reachedDepthOneSemaphore.release();
    searchingSemaphore.release();

#ifdef wasm
    printf("Depth: %zu\n", lastPV.size());
    int score;
    if (tt.contains(board.hashCode)) {
        const TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
        score = entry.score;
    } else {
        score = Evaluator::evaluate(board);
    }
    printf("Evaluation: %d\nPV: ", score);
    printf("\nBoard hash: %llu", board.hashCode);
    printf("\nTT reads: %d", tt.reads);
    printf("\nTT writes: %d", tt.writes);
    printf("\nTT collisions: %d", tt.collisions);
    printf("\nTT occupancy: %d", tt.entryCount);
    printf("\n**************************\n");
    logger.sendInt("updateTTOccupancy", tt.entryCount);
    logger.sendInt("updateEvaluation", score);
#endif
    tt.resetCounters();

    lastPV = collectPV(lastPV.size() + 1);
    if (lastPV.empty()) {
        auto entry = tt.getEntry(board.hashCode, 0);
        std::cout << "problem HASH: " << board.hashCode << " TT: key- " << entry.key << ", move- " << entry.bestMove
                  << ", type- " << entry.nodeType <<
                  ", depth- " << entry.depth << ", score- " << entry.score << std::endl;
        return NULL_MOVE;
    }

    logger.end();

    return lastPV[0];
}

void Search::threadedSearch() {
    constexpr int alpha = INT32_MIN + 1;
    constexpr int beta = INT32_MAX;

    logger.log(std::format("info depth 1\n"));
    logger.logToFile(std::format("starting depth 1\n"));

    logger.logToFile("root begin\n");
    alphaBeta(1, alpha, beta, 0);
    logger.logToFile("root end\n");

    bool endEarly = false;

    lastPV = collectPV(1, endEarly);
    reachedDepthOneSemaphore.release();

    int i = std::max((int) lastPV.size(), 2);

    for (; i < 64 && !endEarly; ++i) {
        logger.log(std::format("info depth {}\n", i));
        logger.logToFile(std::format("starting depth {}\n", i));

        logger.logToFile("root begin\n");
        alphaBeta(i, alpha, beta, 0);
        logger.logToFile("root end\n");
        if (stop) break;

        lastPV = collectPV(i, endEarly);

        if (tt.contains(board.hashCode)) {
            const TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
            logger.sendInt("updateEvaluation", entry.score);
        }
        logger.sendInt("updateDepth", i);
        logger.sendInt("updateTTOccupancy", tt.entryCount);


        i = std::max((int) lastPV.size(), i);
    }
    searchingSemaphore.release();
}

int Search::alphaBeta(const int depth, int alpha, int beta, const int ply) {
    if (stop) { return 0; }

    if (ply > 0) {
        //repetitions
        if (board.isDraw()) {
            logger.logToFile("draw, score: 0\n");
            return Util::randomOffset();
        }

        alpha = std::max(alpha, -MATE_SCORE + ply);
        beta = std::min(beta, MATE_SCORE - ply);
        if (alpha >= beta) return alpha;
    }

    if (depth < 1) return quiesce(alpha, beta, ply, 0);

    Move hashMove = NULL_MOVE;
    int positionScore = 0;
    if (getTransposition(board.hashCode, depth, ply, positionScore, alpha, beta, hashMove)) {
        logger.logToFile(std::format("transposition found | depth: {} score: {}\n", depth, positionScore));
        return positionScore;
    }

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
        logger.logToFile(move.toString() + " begin\n");

        hasLegalMoves = true;

        int score = 0;

        // late move reductions
        bool shouldFullSearch = true;
        if (i > 2 && depth > 3 && !move.tactical()) {
            score = -alphaBeta(depth - 2, -alpha - 1, -alpha, ply + 1);
            shouldFullSearch = score > alpha;
        }

        if (shouldFullSearch) score = -alphaBeta(depth - 1, -beta, -alpha, ply + 1);
        board.unMakeMove();

        logger.logToFile(std::format("{} end score : {}\n", move.toString(), score));

        if (stop) return 0;

        if (score >= beta) {
            if (move.flag == 0 || move.flag >= 7) {
                storeKillerMove(move, ply);
                history[board.sideToMove][move.start][move.end] += depth * depth;
            }

            tt.setEntry(board, move, depth, score, TranspositionTable::LOWERBOUND, ply);
            logger.logToFile("beta cutoff\n");
            return score;
        }
        if (score > alpha) {
            alpha = score;
            bestScore = score;
            bestMove = move;
            nodeType = TranspositionTable::EXACT;
        } else if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    if (!hasLegalMoves) {
        if (MoveGenerator::inCheck(board, board.sideToMove)) return -(MATE_SCORE - ply);
        return 0;
    }
    logger.logToFile("bestmove " + bestMove.toString() + "\n");
    tt.setEntry(board, bestMove, depth, alpha, nodeType, ply);

    return alpha;
}

int Search::quiesce(int alpha, int beta, const int ply, const int depth) {
    if (stop) return 0;

    //repetitions
    if (board.isDraw()) return Util::randomOffset();

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

        const int score = -quiesce(-beta, -alpha, ply + 1, depth - 1);
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
        } else if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }
    tt.setEntry(board, bestMove, depth, alpha, nodeType, ply);

    return alpha;
}

std::vector<ScoredMove> Search::scoreMoves(const std::vector<Move> &moves, const int ply, const Move &hashMove) const {
    std::vector<ScoredMove> scoredMoves;
    for (const Move &move: moves) {
        int score = 0;

        if (move == hashMove) score = 1 << 30;

        else if (move.promotionType != 0) {
            score = mg_value[move.promotionType - 1] - mg_value[0];
        } else if (move.flag == 0 || move.flag >= 7) {
            if (move == killerMoves[ply][0] || move == killerMoves[ply][1])
                score = 1 << 14;
            else score = history[board.sideToMove][move.start][move.end];
        } else {
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

std::vector<ScoredMove> Search::scoreTacticalMoves(const std::vector<Move> &moves, const Move &hashMove) const {
    std::vector<ScoredMove> scoredMoves;

    for (const Move &move: moves) {
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

Move Search::selectMove(std::vector<ScoredMove> &moves, const int index) {
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

void Search::storeKillerMove(const Move &move, const int ply) {
    if ((move.flag == 0 || move.flag >= 7) && move.promotionType == 0) {
        if (killerMoves[ply][0] == move) return;
        if (killerMoves[ply][1] == move) return;

        if (killerMoveIndexOne || killerMoves[ply][0] == NULL_MOVE) {
            killerMoves[ply][0] = move;
            killerMoveIndexOne = false;
        } else {
            killerMoves[ply][1] = move;
            killerMoveIndexOne = true;
        }
    }
}

Search::Search(ChessBoard &board) : board(board) {
}

std::vector<Move> Search::collectPV(const int depth, bool &endEarly) const {
    std::vector<Move> pv;
    std::unordered_set<uint64_t> pvPositions;
    std::vector<int> scores;
    pv.reserve(depth);

    int pvDepth = 0;
    while (tt.contains(board.hashCode)) {
        const TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
        if ((entry.nodeType != TranspositionTable::EXACT && entry.nodeType != TranspositionTable::BOOK) ||
            entry.depth < 1)
            break;
        Move move = entry.bestMove;

        if (TranspositionTable::isMateScore(entry.score) || entry.nodeType == TranspositionTable::BOOK)
            endEarly = true;
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

        if (pvPositions.contains(board.hashCode)) {
            std::cout << "Cycle in PV!" << std::endl;
            break;
        }
    }
    std::string pvString;
    for (auto move: pv) {
        pvString += move.toString() + " ";
    }
    logger.log("info PV " + pvString + "\n");
    logger.sendString("updatePV", pvString);

    for (; pvDepth > 0; --pvDepth) {
        board.unMakeMove();
    }

    return pv;
}

std::vector<Move> Search::collectPV(int depth) const {
    bool unused = false;
    return collectPV(depth, unused);
}


bool Search::getTransposition(const uint64_t hash, const int depth, const int ply, int &score, const int &alpha,
                              const int &beta, Move &hashMove) {
    if (tt.contains(hash)) {
        const TranspositionTable::Entry entry = tt.getEntry(hash, ply);

        if (entry.nodeType == TranspositionTable::BOOK) {
            score = entry.score;
            return true;
        }

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
                default:
                    break;
            }
        }
        hashMove = entry.bestMove;
    }

    return false;
}

void Search::reset() {
    stop = false;
    lastPV.clear();
    killerMoves = std::array<std::array<Move, 2>, 64>();
    killerMoveIndexOne = false;
    history = std::array<std::array<std::array<int, 64>, 64>, 2>();
}
