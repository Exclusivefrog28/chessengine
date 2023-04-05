#ifndef CHESSENGINE_CHESSBOARD_H
#define CHESSENGINE_CHESSBOARD_H


#include <vector>
#include <iostream>
#include "Move.h"
#include "Piece.h"

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
    short enPassantFile = 0;

    std::vector<Move> history;

    ChessBoard();

    void setStartingPosition();

    friend std::ostream& operator<<(std::ostream& os, const ChessBoard& board);

    void makeMove(Move move);
    void unMakeMove();

private:
    static Color invertColor(Color color);

    void movePiece(short start ,short end);
    void setPiece(short position, Square piece);
    void removePiece(short position);

};


#endif //CHESSENGINE_CHESSBOARD_H
