#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"
#include <unordered_set>

#define MATE_SCORE 65536

TranspositionTable Search::tt = TranspositionTable();

Moves::Move Search::search(ChessBoard &board, int timeOut) {

    int alpha = INT32_MIN + 1;
    int beta = INT32_MAX;

    Search search = Search(board);

    std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
    std::chrono::seconds timeLimit(timeOut);

    int i = 1;

    for (;; ++i) {
        if (i > 1) {
            std::chrono::duration<double> timeSpent = std::chrono::steady_clock::now() - startTime;
            bool gameOver = false;
            search.lastPV = search.collectPV(i - 1, gameOver);
            if (gameOver) break;

//            double branchingFactor = static_cast<double>(search.nodeCount) / search.previousNodeCount;
//            search.previousNodeCount = search.nodeCount;
//            double predictedNodes = search.nodeCount * branchingFactor;
//            std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - startTime;
//            std::chrono::duration<double> predictedTime =
//                    elapsedTime * (static_cast<double>(predictedNodes / search.nodeCount));
//
//            if (elapsedTime + predictedTime > timeLimit) break;

            if ((timeSpent * 2) > timeLimit) break;
        }
        search.alphaBeta(i, alpha, beta, 0);
    }

    printf("Depth: %d\nPV: ", i - 1);
    for (const Move &move: search.lastPV) {
        printf("%s%s ", Util::positionToString(move.start).c_str(), Util::positionToString(move.end).c_str());
    }
    printf("\nTT reads: %d", tt.reads);
    printf("\nTT writes: %d", tt.writes);
    printf("\nTT collisions: %d", tt.collisions);
    printf("\n**************************\n");
    tt.resetCounters();

    return search.lastPV[0];
}

int Search::alphaBeta(int depth, int alpha, int beta, int ply) {
    if (depth == 0) return quiesce(alpha, beta, ply, 0);

    Move hashMove{};
    int positionScore = 0;
    if (getTransposition(board.hashCode, depth, ply, positionScore, alpha, beta, hashMove)) return positionScore;

    alpha = std::max(alpha, -MATE_SCORE + ply);
    beta = std::min(beta, MATE_SCORE - ply);
    if (alpha >= beta) return alpha;

    std::vector<ScoredMove> moves = scoreMoves(MoveGenerator::pseudoLegalMoves(board), ply, hashMove);

    bool hasLegalMoves = false;

    TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
    Move bestMove{};
    int bestScore = INT32_MIN;

    for (int i = 0; i < moves.size(); i++) {
        Move move = selectMove(moves, i);

        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }

        hasLegalMoves = true;

        bool draw = false;
        //50 move rule
        if (board.halfMoveClock >= 100 &&
            !(board.squares[move.start].type == PAWN || (move.flag >= 1 && move.flag <= 5)))
            draw = true;
        //threefold repetition
        else {
            int repetitions = 1;
            for (int j = board.positionHistory.size() - 1;
                 j >= 0 && (board.irreversibleIndices.empty() || board.irreversibleIndices.back() < j);
                 --j) {
                if (board.positionHistory[j] == board.hashCode) repetitions++;
                if (repetitions == 3) {
                    draw = true;
                    break;
                }
            }
        }

        int score = (draw) ? 0 : -alphaBeta(depth - 1, -beta, -alpha, ply + 1);

        board.unMakeMove();

        if (score >= beta) {
            if (move.flag == 0 || move.flag >= 7) {
                storeKillerMove(move, ply);
                history[board.sideToMove][move.start][move.end] += depth * depth;
            }

            tt.setEntry(board.hashCode,
                        {board.hashCode, move, depth, score, TranspositionTable::LOWERBOUND}, ply);
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
        else return 0;
    } else {
        tt.setEntry(board.hashCode, {board.hashCode, bestMove, depth, bestScore, nodeType}, ply);
    }

    return alpha;
}

int Search::quiesce(int alpha, int beta, int ply, int depth) {
    int stand_pat = Evaluator::evaluate(board);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    if (stand_pat + EvaluationValues::mg_value[QUEEN - 1] < alpha) {
        return alpha;
    }

    Move hashMove{};
    int positionScore = 0;

    if (getTransposition(board.hashCode, depth, ply, positionScore, alpha, beta, hashMove)) return positionScore;

    std::vector<ScoredMove> moves = scoreTacticalMoves(MoveGenerator::tacticalMoves(board), hashMove);

    TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
    Move bestMove{};
    int bestScore = INT32_MIN;

    for (int i = 0; i < moves.size(); i++) {
        Move move = selectMove(moves, i);

        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }
        int score = -quiesce(-beta, -alpha, ply + 1, depth - 1);
        board.unMakeMove();

        if (score >= beta) {
            tt.setEntry(board.hashCode, {board.hashCode, move, depth, score, TranspositionTable::LOWERBOUND}, ply);
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
    tt.setEntry(board.hashCode, {board.hashCode, bestMove, depth, bestScore, nodeType}, ply);

    return alpha;
}

std::vector<ScoredMove> Search::scoreMoves(const std::vector<Move> &moves, int ply, Move hashMove) const {

    std::vector<ScoredMove> scoredMoves;

    for (const Move &move: moves) {
        int score = 0;
        int captureScore = 0;

        if (lastPV.size() - 1 <= ply && lastPV[ply] == move) score = 1 << 31;

        else if (move == hashMove) score = 1 << 30;

        else if (move.promotionType != 0) {
            score = EvaluationValues::mg_value[move.promotionType - 1] - EvaluationValues::mg_value[0];
        } else if (move.flag == 0 || move.flag >= 7) {
            if (move == killerMoves[ply][0] || move == killerMoves[ply][1])
                score = 1 << 14;
            else score = history[board.sideToMove][move.start][move.end];
        } else {
            if (move.flag == 6) score = 1 << 16;
            else {
                int agressor = EvaluationValues::mg_value[board.squares[move.start].type - 1];
                int victim = EvaluationValues::mg_value[move.flag - 1];
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

std::vector<ScoredMove> Search::scoreTacticalMoves(const std::vector<Move> &moves, Move hashMove) const {

    std::vector<ScoredMove> scoredMoves;


    for (const Move &move: moves) {
        int score;

        if (move == hashMove) score = 1 << 30;
        else {
            if (move.promotionType != 0)
                score = EvaluationValues::mg_value[move.promotionType - 1] - EvaluationValues::mg_value[0];
            else
                score = EvaluationValues::mg_value[move.flag - 1] -
                        EvaluationValues::mg_value[board.squares[move.start].type - 1];
        }

        scoredMoves.push_back({move, score});
    }
    return scoredMoves;
}

Move Search::selectMove(std::vector<ScoredMove> &moves, int index) {

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
        } else {
            killerMoves[ply][1] = move;
            killerMoveIndexOne = true;
        }
    }
}

Search::Search(ChessBoard &board) : board(board) {}

std::vector<Move> Search::collectPV(int depth, bool &gameOver) {
    std::vector<Move> pv;
    std::unordered_set<unsigned long> pvPositions;
    pv.reserve(depth);

    int pvDepth = 0;
    while (tt.contains(board.hashCode) && pvPositions.find(board.hashCode) == pvPositions.end()) {
        TranspositionTable::Entry entry = tt.getEntry(board.hashCode, 0);
        if (entry.nodeType != TranspositionTable::EXACT) break;
        if (abs(entry.score) == MATE_SCORE) gameOver = true;
        Move move = entry.bestMove;
        pvPositions.insert(board.hashCode);
        board.makeMove(move);
        pv.push_back(move);
        pvDepth++;
    }
    for (; pvDepth > 0; --pvDepth) {
        board.unMakeMove();
    }

    return pv;
}

bool Search::getTransposition(unsigned long hash, int depth, int ply, int &score, int &alpha, int &beta,
                              Move &hashMove) const {

    if (tt.contains(hash)) {
        TranspositionTable::Entry entry = tt.getEntry(hash, ply);
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
                    hashMove = entry.bestMove;
                    beta = std::min(beta, entry.score);
                    break;
                case TranspositionTable::LOWERBOUND:
                    if (entry.score >= beta) {
                        score = entry.score;
                        return true;
                    }
                    hashMove = entry.bestMove;
                    alpha = std::max(alpha, entry.score);
            }
        }
    }

    return false;
}
