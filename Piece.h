#ifndef CHESSENGINE_PIECE_H
#define CHESSENGINE_PIECE_H

namespace Pieces {

    enum Type {
        EMPTY = 0,
        PAWN = 1,
        KNIGHT = 2,
        BISHOP = 3,
        ROOK = 4,
        QUEEN = 5,
        KING = 6
    };

    enum Color {
        WHITE = 0,
        BLACK = 1
    };

    struct Piece {
        Type type;
        short position;
    };
}

#endif //CHESSENGINE_PIECE_H
