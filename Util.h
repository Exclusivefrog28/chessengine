#ifndef CHESSENGINE_UTIL_H
#define CHESSENGINE_UTIL_H

#include "Piece.h"

namespace Util {

    using namespace Piece;

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

    static std::string positionToString(short position){
        short rank = 8 - (position / 8);
        short file = position % 8;

        std::string files[] = {"a", "b", "c", "d", "e", "f", "g", "h"};

        return files[file] + std::to_string(rank);
    }


}

#endif //CHESSENGINE_UTIL_H
