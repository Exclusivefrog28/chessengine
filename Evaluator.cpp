#include "Evaluator.h"

int Evaluator::evaluate(const ChessBoard &board) {

    int score = 0;

    score += materialScore(board, board.sideToMove);
    score -= materialScore(board, Util::invertColor(board.sideToMove));

    return score;
}

int Evaluator::materialScore(const ChessBoard &board, Color side) {
    const std::vector<Piece> *pieces = (side == WHITE) ? &board.whitePieces : &board.blackPieces;
    const std::vector<short> *pawns = (side == WHITE) ? &board.whitePawns : &board.blackPawns;

    int score = 0;
    for (Piece piece : *pieces) {
        score += materialValues[piece.type - 1];
    }
    score += materialValues[0] * pawns->size();

    return score;
}
