#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"

#define MATE_SCORE 65536

Moves::Move Search::search(ChessBoard &board, int timeOut) {

    int alpha = INT32_MIN + 1;
    int beta = INT32_MAX;

    Search search = Search(board);

    std::chrono::time_point<std::chrono::steady_clock> startTime = std::chrono::steady_clock::now();
    std::chrono::seconds timeLimit(timeOut);

    int i = 1;

    for (;; ++i) {
        if (i > 1) {
            if(search.principalVariation.size() <= search.lastPV.size()) break;

            search.lastPV = search.principalVariation;

            double branchingFactor = static_cast<double>(search.nodeCount) / search.previousNodeCount;
            search.previousNodeCount = search.nodeCount;
            double predictedNodes = search.nodeCount * branchingFactor;
            std::chrono::duration<double> elapsedTime = std::chrono::steady_clock::now() - startTime;
            std::chrono::duration<double> predictedTime =
                    elapsedTime * (static_cast<double>(predictedNodes / search.nodeCount));

            if (elapsedTime + predictedTime > timeLimit) break;
        }

        search.principalVariation.clear();
        search.nodeCount = 0;
        search.alphaBeta(i, alpha, beta, 0, search.principalVariation);
    }

    printf("Depth: %d PV: ", i - 1);
    for (const Move &move: search.principalVariation) {
        printf("%s%s ", Util::positionToString(move.start).c_str(), Util::positionToString(move.end).c_str());
    }
    printf("\n");

    return search.principalVariation[0];
}

int Search::alphaBeta(int depth, int alpha, int beta, int ply, std::vector<Move> &pv) {
    if (depth == 0) return quiesce(alpha, beta, ply, pv);

    Move *hashMove = nullptr;

    if (TranspositionTable::contains(board.hashCode)) {
        TranspositionTable::Entry entry = TranspositionTable::getEntry(board.hashCode, ply);
        if (entry.depth >= depth) {
            switch (entry.nodeType) {
                case TranspositionTable::EXACT:
                    pv.push_back(entry.bestMove);
                    return entry.score;
                case TranspositionTable::UPPERBOUND:
                    if (entry.score <= alpha) return entry.score;
                    beta = std::min(beta, entry.score);
                    break;
                case TranspositionTable::LOWERBOUND:
                    if (entry.score >= beta) {
                        pv.push_back(entry.bestMove);
                        return entry.score;
                    }
                    hashMove = &entry.bestMove;
                    alpha = std::max(alpha, entry.score);
            }
        }
    }

    alpha = std::max(alpha, -MATE_SCORE + ply);
    beta = std::min(beta, MATE_SCORE - ply);
    if (alpha >= beta) return alpha;

    std::vector<ScoredMove> moves = scoreMoves(MoveGenerator::pseudoLegalMoves(board), ply, hashMove);

    bool hasLegalMoves = false;

    TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
    Move bestMove{};

    for (int i = 0; i < moves.size(); i++) {
        Move move = selectMove(moves, i);
        int score;

        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }

        std::vector<Move> childPV;
        hasLegalMoves = true;


        if (board.halfMoveClock >= 100 &&
            !(board.squares[move.start].type == PAWN || (move.flag >= 1 && move.flag <= 5)))
            score = 0;
        else score = -alphaBeta(depth - 1, -beta, -alpha, ply + 1, childPV);

        board.unMakeMove();

        if (score >= beta) {
            if (move.flag == 0 || move.flag >= 7) {
                storeKillerMove(move, ply);
                history[board.sideToMove][move.start][move.end] += depth * depth;
            }

            TranspositionTable::setEntry(board.hashCode,
                                         {board.hashCode, move, depth, score, TranspositionTable::LOWERBOUND});
            return score;
        }
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            nodeType = TranspositionTable::EXACT;

            pv.clear();
            pv.push_back(move);
            std::copy(childPV.begin(), childPV.end(), back_inserter(pv));
        }
    }
    if (!hasLegalMoves) {
        nodeCount++;
        if (MoveGenerator::inCheck(board, board.sideToMove)) return -(MATE_SCORE - ply);
        else return 0;
    } else {
        TranspositionTable::setEntry(board.hashCode, {board.hashCode, bestMove, depth, alpha, nodeType});
    }

    return alpha;
}

int Search::quiesce(int alpha, int beta, int ply, std::vector<Move> &pv) {
    nodeCount++;
    int stand_pat = Evaluator::evaluate(board);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    if (stand_pat + EvaluationValues::mg_value[QUEEN - 1] < alpha) {
        return alpha;
    }

    std::vector<ScoredMove> moves = scoreTacticalMoves(MoveGenerator::tacticalMoves(board));

    for (int i = 0; i < moves.size(); i++) {
        Move move = selectMove(moves, i);

        std::vector<Move> childPV;
        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }
        int score = -quiesce(-beta, -alpha, ply + 1, childPV);
        board.unMakeMove();

        if (score >= beta) {
            return score;
        }
        if (score > alpha) {
            alpha = score;
            pv.clear();
            pv.push_back(move);
            std::copy(childPV.begin(), childPV.end(), back_inserter(pv));
        }
    }

    return alpha;
}

std::vector<ScoredMove> Search::scoreMoves(const std::vector<Move> &moves, int ply, Move *hashMove) const {

    std::vector<ScoredMove> scoredMoves;

    for (const Move &move: moves) {
        int score = 0;
        int captureScore = 0;

        if (lastPV.size() - 1 <= ply && lastPV[ply] == move) score = 1 << 31;

        else if (hashMove != nullptr && move == *hashMove) score = 1 << 30;

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

std::vector<ScoredMove> Search::scoreTacticalMoves(const std::vector<Move> &moves) const {

    std::vector<ScoredMove> scoredMoves;

    for (const Move &move: moves) {
        int score;
        if (move.promotionType != 0)
            score = EvaluationValues::mg_value[move.promotionType - 1] - EvaluationValues::mg_value[0];
        else
            score = EvaluationValues::mg_value[move.flag - 1] -
                    EvaluationValues::mg_value[board.squares[move.start].type - 1];

        scoredMoves.push_back({move, score});
    }
    return scoredMoves;
}

Move Search::selectMove(std::vector<ScoredMove> &moves, int index) const {

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
