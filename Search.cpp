#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"
#include "TranspositionTable.h"

#define MATE_SCORE 65536

Moves::Move Search::search(ChessBoard &board, int depth) {

    int alpha = INT32_MIN + 1;
    int beta = INT32_MAX;

    Search search = Search(depth, board);

    search.principalVariation.clear();

    search.alphaBeta(depth, alpha, beta, 0, search.principalVariation);

    for (const Move &move: search.principalVariation) {
        printf(" %s%s", Util::positionToString(move.start).c_str(), Util::positionToString(move.end).c_str());
    }
    printf("\n");

    return search.principalVariation[0];
}

int Search::alphaBeta(int depth, int alpha, int beta, int ply, std::vector<Move> &pv) {
    if (depth == 0) return quiescence(alpha, beta, ply, pv);

    if (TranspositionTable::contains(board.hashCode)) {
        TranspositionTable::Entry entry = TranspositionTable::getEntry(board.hashCode, ply);
        if (entry.depth >= depth) {
            if (entry.nodeType == TranspositionTable::EXACT) {
                pv.clear();
                pv.push_back(entry.bestMove);
                if(entry.score >= beta) return beta;
                if(entry.score <= alpha) return alpha;
                return entry.score;
            }
            if (entry.nodeType == TranspositionTable::UPPERBOUND && entry.score <= alpha) {
                pv.clear();
                pv.push_back(entry.bestMove);
                return alpha;
            }
            if (entry.nodeType == TranspositionTable::LOWERBOUND && entry.score >= beta) {
                pv.clear();
                pv.push_back(entry.bestMove);
                return beta;
            }
        }
    }

    alpha = std::max(alpha, -MATE_SCORE + ply);
    beta = std::min(beta, MATE_SCORE - ply);
    if (alpha >= beta) return alpha;

    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

    bool hasLegalMoves = false;

    std::sort(moves.begin(), moves.end(), [this, ply](const Move &a, const Move &b) {
        return scoreMove(a, ply + 1) > scoreMove(b, ply + 1);
    });

    TranspositionTable::NodeType nodeType = TranspositionTable::UPPERBOUND;
    Move bestMove{};

    for (const Move &move: moves) {

        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }

        std::vector<Move> childPV;
        hasLegalMoves = true;

        int score = -alphaBeta(depth - 1, -beta, -alpha, ply + 1, childPV);
        board.unMakeMove();

        if (score >= beta) {
            storeKillerMove(move, ply);
            TranspositionTable::setEntry(board.hashCode,
                                         {board.hashCode, move, depth, beta, TranspositionTable::LOWERBOUND});
            return beta;
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
        if (MoveGenerator::inCheck(board, board.sideToMove)) return -(MATE_SCORE - ply);
        else return 0;
    } else {
        TranspositionTable::setEntry(board.hashCode, {board.hashCode, bestMove, depth, alpha, nodeType});
    }

    return alpha;
}

int Search::quiescence(int alpha, int beta, int ply, std::vector<Move> &pv) {
    int stand_pat = Evaluator::evaluate(board);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;
    if (stand_pat + EvaluationValues::mg_value[QUEEN - 1] < alpha) {
        return alpha;
    }


    std::vector<Move> moves = MoveGenerator::tacticalMoves(board);
    std::sort(moves.begin(), moves.end(), [this](const Move &a, const Move &b) {
        return scoreMove(a, 0) > scoreMove(b, 0);
    });

    for (const Move &move: moves) {
        std::vector<Move> childPV;
        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }
        int score = -quiescence(-beta, -alpha, ply + 1, childPV);
        board.unMakeMove();

        if (score >= beta)
            return beta;
        if (score > alpha) {
            alpha = score;
            pv.clear();
            pv.push_back(move);
            std::copy(childPV.begin(), childPV.end(), back_inserter(pv));
        }
    }
    return alpha;
}

int Search::scoreMove(const Move &move, int ply) const {

    Square piece = board.squares[move.start];

    int score = 0;

    if (move.promotionType != 0) {
        score += EvaluationValues::mg_value[move.promotionType - 1] - EvaluationValues::mg_value[0];
    } else if (move.flag == 0) {
        if (move == killerMoves[ply][0] || move == killerMoves[ply][1] || move == killerMoves[ply][2])
            score += 10;
    }

    if (move.flag >= 1 && move.flag <= 5) {
        int agressor = EvaluationValues::mg_value[piece.type - 1];
        int victim = EvaluationValues::mg_value[move.flag - 1];
        score += victim - agressor;
    }

    return score;
}

Search::Search(int depth, ChessBoard &board) : board(board) {
    killerMoves = new std::array<Move, 3>[depth];
}

Search::~Search() {
    delete[] killerMoves;
}

void Search::storeKillerMove(const Move &move, int ply) const {
    if (move.flag == 0 && move.promotionType == 0) {
        if (killerMoves[ply][0] == NULL_MOVE) {
            killerMoves[ply][0] = move;
            return;
        }
        if (killerMoves[ply][1] == NULL_MOVE) {
            killerMoves[ply][1] = move;
            return;
        }
        if (killerMoves[ply][2] == NULL_MOVE) {
            killerMoves[ply][2] = move;
            return;
        }
        if (killerMoves[ply][0] != move) {
            killerMoves[ply][0] = move;
            return;
        }
        if (killerMoves[ply][1] != move) {
            killerMoves[ply][1] = move;
            return;
        }
        if (killerMoves[ply][2] != move) {
            killerMoves[ply][2] = move;
            return;
        }
    }
}
