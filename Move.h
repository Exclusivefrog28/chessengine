#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H

#include "Piece.h"

namespace Moves {

    using Piece::Color, Piece::Type;

    enum MoveFlag {
        QUIET = 0,
        CAPTUREPAWN = 1,
        CAPTUREKNIGHT = 2,
        CAPTUREBISHOP = 3,
        CAPTUREROOK = 4,
        CAPTUREQUEEN = 5,
        ENPASSANT = 6,
        DOUBLEPAWNMOVE = 7,
        CASTLEKINGSIDE = 8,
        CASTLEQUEENSIDE = 9,
    };

    struct Move {
        short start;
        short end;
        Type promotionType;
        MoveFlag flag;
        Color player;
    };

}
#endif //CHESSENGINE_MOVE_H
