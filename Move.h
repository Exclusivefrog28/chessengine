#ifndef CHESSENGINE_MOVE_H
#define CHESSENGINE_MOVE_H

#include "Piece.h"

enum MoveFlag{
    QUIET = 0,
    CAPTUREPAWN = 1,
    CAPTUREKNIGHT = 2,
    CAPTUREBISHOP = 3,
    CAPTUREROOK = 4,
    CAPTUREQUEEN = 5,
    ENPASSANT = 6,
    CASTLEKINGSIDE = 7,
    CASTLEQUEENSIDE = 8,
    PROMOTEQUEEN = 9,
    PROMOTEROOK = 10,
    PROMOTEBISHOP = 11,
    PROMOTEKNIGHT = 12
};


struct Move {
    short start;
    short end;
    MoveFlag flag;
    Color player;
};

#endif //CHESSENGINE_MOVE_H
