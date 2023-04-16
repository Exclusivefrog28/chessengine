#ifndef CHESSENGINE_CHESSBOARD_H
#define CHESSENGINE_CHESSBOARD_H


#include <vector>
#include <iostream>
#include "Move.h"
#include "Piece.h"
#include "Util.h"

using namespace Pieces;
using namespace Moves;

class ChessBoard {

public:

    struct CastlingRights {
        bool blackKingSide;
        bool blackQueenSide;
        bool whiteKingSide;
        bool whiteQueenSide;
    };


    std::array<Square, 64> squares;

    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;

    Color sideToMove = WHITE;
    CastlingRights castlingRights{};
    short enPassantSquare = -1;

    int halfMoveClock = 0;
    int fullMoveClock = 1;

    std::vector<Move> moveHistory;
    std::vector<CastlingRights> castlingRightHistory;
    std::vector<short> enPassantHistory;

    ChessBoard();

    void setStartingPosition();

    void setPosition(std::string fen);

    friend std::ostream &operator<<(std::ostream &os, const ChessBoard &board);

    std::string fen();

    void makeMove(Move move);

    void unMakeMove();

private:
    void movePiece(short start, short end);

    void setPiece(short position, Square piece);

    void removePiece(short position);

    void updateCastlingRights(Move move);

};


#endif //CHESSENGINE_CHESSBOARD_H
