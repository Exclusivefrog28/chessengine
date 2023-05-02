#include "Search.h"
#include "Evaluator.h"
#include "MoveGenerator.h"

Moves::Move Search::search(ChessBoard &board, int depth) {

    std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

    int alpha = INT32_MIN + 1;
    int beta = INT32_MAX;

    Move bestMove{};
    for (Move &move: moves) {
        board.makeMove(move);
        if (MoveGenerator::inCheck(board, invertColor(board.sideToMove))) {
            board.unMakeMove();
            continue;
        }
        int score = -alphaBeta(board, depth - 1, alpha, beta);
        board.unMakeMove();

        if (score > alpha){
            bestMove = move;
            alpha = score;
        }
    }

    return bestMove;
}

int Search::alphaBeta(ChessBoard &board, int depth, int alpha, int beta) {
    if (depth == 0) return quiescence(board, alpha, beta);

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

int Search::quiescence(ChessBoard &board, int alpha, int beta) {
    int stand_pat = Evaluator::evaluate(board);
    if( stand_pat >= beta )
        return beta;
    if( alpha < stand_pat )
        alpha = stand_pat;

    std::vector<Move> moves = MoveGenerator::tacticalMoves(board);

    for (const Move &move: moves){
        board.makeMove(move);
        if( MoveGenerator::inCheck(board, invertColor(board.sideToMove)) ){
            board.unMakeMove();
            continue;
        }
        int score = -quiescence(board, -beta, -alpha);
        board.unMakeMove();

        if( score >= beta )
            return beta;
        if( score > alpha )
            alpha = score;

    }
    return alpha;
}
