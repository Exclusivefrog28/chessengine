#ifndef CHESSENGINE_MOVEPARSER_H
#define CHESSENGINE_MOVEPARSER_H

#include "Move.h"
#include "ChessBoard.h"

static Move parseMove(const std::string&string, const ChessBoard&board) {
    const int startPos = Util::stringToPosition(string.substr(0, 2));
    const int endPos = Util::stringToPosition(string.substr(2, 2));
    Type promotionType = EMPTY;
    MoveFlag flag = QUIET;

    if (string.length() > 4) {
        promotionType = Util::charToPiece(string[4]).type;
    }
    const Square&startSquare = board.squares[startPos];
    const Square&endSquare = board.squares[endPos];
    if (endSquare.type != EMPTY && endSquare.color != board.sideToMove) {
        flag = static_cast<MoveFlag>(endSquare.type);
    }
    else {
        if (startSquare.type == PAWN) {
            if (startPos / 8 == 1 && endPos / 8 == 3 || startPos / 8 == 6 && endPos / 8 == 4) {
                flag = DOUBLEPAWNPUSH;
            }
            else if (abs(startPos - endPos) == 7 || abs(startPos - endPos) == 9) {
                flag = ENPASSANT;
            }
        }
        else {
            if (startSquare.type == KING) {
                if (startPos == 4 && startSquare.color == BLACK) {
                    if (endPos == 2) flag = CASTLEQUEENSIDE;
                    else if (endPos == 6) flag = CASTLEKINGSIDE;
                }
                if (startPos == 60 && startSquare.color == WHITE) {
                    if (endPos == 58) flag = CASTLEQUEENSIDE;
                    else if (endPos == 62) flag = CASTLEKINGSIDE;
                }
            }
        }
    }

    return Move(startPos, endPos, promotionType, flag, board.sideToMove);
}

#endif //CHESSENGINE_MOVEPARSER_H
