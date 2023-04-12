#ifndef CHESSENGINE_CHESSBOARD_H
#define CHESSENGINE_CHESSBOARD_H


#include <vector>
#include <iostream>
#include "Move.h"
#include "Piece.h"
#include "Util.h"

using namespace Piece;
using namespace Moves;

class ChessBoard {

public:


    struct Square {
        Type type;
        Color color;
    };

    struct Piece {
        Type type;
        short position;
    };

    struct CastlingRights {
        bool blackShort;
        bool blackLong;
        bool whiteShort;
        bool whiteLong;
    };


    Square squares[64]{};

    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;

    Color sideToMove = WHITE;
    CastlingRights castlingRights{};
    short enPassantSquare = -1;

    int halfMoveClock = 0;
    int fullMoveClock = 1;

    std::vector<Move> moveHistory;
    std::vector<CastlingRights> castlingRightHistory;

    ChessBoard();

    void setStartingPosition();

    friend std::ostream& operator<<(std::ostream& os, const ChessBoard& board);
    std::string fen();

    void makeMove(Move move);
    void unMakeMove();

private:
    static Color invertColor(Color color);

    void movePiece(short start ,short end);
    void setPiece(short position, Square piece);
    void removePiece(short position);

    void updateCastlingRights(Move move);

};


#endif //CHESSENGINE_CHESSBOARD_H
