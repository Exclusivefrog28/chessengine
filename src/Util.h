#ifndef CHESSENGINE_UTIL_H
#define CHESSENGINE_UTIL_H

#include <array>
#include <algorithm>
#include "Piece.h"

namespace Util {

    using namespace Pieces;

    static std::string pieceToString(const Type type, const Color color) {
        std::string piece;
        switch (type) {
            case PAWN:
                piece = 'p';
                break;
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
            case EMPTY:
                break;
        }
        if (color == WHITE) std::ranges::transform(piece, piece.begin(), ::toupper);
        return piece;
    }

    static Square charToPiece(const char c) {
        Square p;
        const char C = c;
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

    static std::string positionToString(const int_fast8_t position) {
        const int_fast8_t rank = 8 - (position / 8);
        const int_fast8_t file = position % 8;

        const std::array<std::string, 8> files = {"a", "b", "c", "d", "e", "f", "g", "h"};

        return files[file] + std::to_string(rank);
    }

    static int_fast8_t stringToPosition(const std::string&position) {
        const int_fast8_t file = position[0] - 'a';
        const int_fast8_t rank = 8 - (position[1] - '0');
        return rank * 8 + file;
    }


}

#endif //CHESSENGINE_UTIL_H
