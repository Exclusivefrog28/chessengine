#ifndef CHESSENGINE_PIECE_H
#define CHESSENGINE_PIECE_H

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

static std::string pieceToString(Type type, Color color) {
    std::string piece = "p";
    switch (type) {
        case KNIGHT:
            piece = 'n';
            break;
        case BISHOP:
            piece = 'b';
            break;
        case ROOK:
            piece = 'r';
            break;
        case QUEEN:
            piece = 'q';
            break;
        case KING:
            piece = 'k';
            break;
    }
    if (color == WHITE) std::transform(piece.begin(), piece.end(), piece.begin(), ::toupper);
    return piece;
}


#endif //CHESSENGINE_PIECE_H
