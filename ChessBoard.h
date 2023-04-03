#ifndef CHESSENGINE_CHESSBOARD_H
#define CHESSENGINE_CHESSBOARD_H


#include <vector>

class ChessBoard {

public:

    enum PieceType {
        EMPTY = 0,
        PAWN = 1,
        KNIGHT = 2,
        BISHOP = 3,
        ROOK = 4,
        QUEEN = 5,
        KING = 6
    };

    enum PieceColor {
        WHITE = 0,
        BLACK = 1
    };

    struct Square {
        PieceType type;
        PieceColor color;
    };

    struct Piece {
        PieceType type;
        short position;
    };

    struct CastlingRights {
        bool blackShort;
        bool blackLong;
        bool whiteShort;
        bool whiteLong;
    };


    Square squares [64]{};

    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;

    PieceColor sideToMove = WHITE;
    CastlingRights castlingRights{};
    short enPassantFile = 0;


    ChessBoard();

    void setStartingPosition();

};


#endif //CHESSENGINE_CHESSBOARD_H
