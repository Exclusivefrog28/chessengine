#ifndef CHESSENGINE_MOVEGENERATOR_H
#define CHESSENGINE_MOVEGENERATOR_H

#include "Move.h"
#include "ChessBoard.h"
#include <mutex>

class MoveGenerator {
public:
    static std::vector<Move> pseudoLegalMoves(const ChessBoard& board);
    static std::vector<Move> tacticalMoves(const ChessBoard& board);
    static bool isSquareAttacked(const ChessBoard& board, int_fast8_t square, Color color);
    static bool inCheck(const ChessBoard& board, Color color);
    static unsigned long long perft(int depth,  ChessBoard& board);
    static bool isLegalMove(ChessBoard &board, Move move);

private:
    ChessBoard board;
    std::vector<Move> moves;
    std::mutex movesMutex;

    void addMove(const Move& move);

    void generateMovesForPieces(const std::vector<Piece>& pieces);
    void generateMovesForPawns(const std::vector<int_fast8_t>& pawnPositions);
};

#endif //CHESSENGINE_MOVEGENERATOR_H
