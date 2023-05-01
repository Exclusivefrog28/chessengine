#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"

Moves::Move Search::search(ChessBoard &board, int depth) {

    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);
    int bestScore = INT32_MIN + 1;
    Move bestMove{};
    for (Move &move: moves) {
        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }
        int score = -alphaBeta(board, depth - 1, INT32_MIN + 1, INT32_MAX);
        board.unMakeMove();

        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int Search::alphaBeta(ChessBoard &board, int depth, int alpha, int beta) {
    if (depth == 0) return Evaluator::evaluate(board);

    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

    for (Move &move: moves) {
        board.makeMove(move);

        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
        } else {
            int score = -alphaBeta(board, depth - 1, -beta, -alpha);
            board.unMakeMove();

            if (score >= beta)
                return beta;
            if (score > alpha)
                alpha = score;
        }
    }
    return alpha;
}
