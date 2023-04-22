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


    std::array<Square, 64> squares{};

    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;

    std::vector<short> whitePawns;
    std::vector<short> blackPawns;

    short whiteKing{};
    short blackKing{};

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

    void makeMove(const Move &move);

    void unMakeMove();

private:
    void movePiece(const short &start, const short &end);

    void setPiece(const short &position, const Square piece);

    void removePiece(const short &position);

    void updateCastlingRights(const Move &move);

};


#endif //CHESSENGINE_CHESSBOARD_H
