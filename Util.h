#ifndef CHESSENGINE_UTIL_H
#define CHESSENGINE_UTIL_H

#include "Piece.h"

namespace Util {

    using namespace Pieces;

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

    static Square charToPiece(char c) {
        Square p;
        char C = c;
        switch (toupper(C)) {
            case 'P':
                p.type = PAWN;
                break;
            case 'N':
                p.type = KNIGHT;
                break;
            case 'B':
                p.type = BISHOP;
                break;
            case 'R':
                p.type = ROOK;
                break;
            case 'Q':
                p.type = QUEEN;
                break;
            case 'K':
                p.type = KING;
                break;
            default:
                p.type = EMPTY;
                break;
        }
        if(c >= 'a' && c <= 'z') p.color = BLACK;
        else p.color = WHITE;

        return p;
    }

    static std::string positionToString(short position) {
        short rank = 8 - (position / 8);
        short file = position % 8;

        std::array<std::string, 8> files = {"a", "b", "c", "d", "e", "f", "g", "h"};

        return files[file] + std::to_string(rank);
    }

    static short stringToPosition(std::string position) {
        short file = position[0] - 'a';
        short rank = 8 - (position[1] - '0');
        return rank * 8 + file;
    }


}

#endif //CHESSENGINE_UTIL_H
