#include "ChessBoard.h"

void ChessBoard::setStartingPosition() {

    for (short i = 0; i < 64; i++) {
        if (i < 16 || i >= 48) {

            switch (i) {
                case 0:
                case 7:
                case 56:
                case 63:
                    squares[i].type = ROOK;
                    break;
                case 1:
                case 6:
                case 57:
                case 62:
                    squares[i].type = KNIGHT;
                    break;
                case 2:
                case 5:
                case 58:
                case 61:
                    squares[i].type = BISHOP;
                    break;
                case 3:
                case 59:
                    squares[i].type = QUEEN;
                    break;
                case 4:
                case 60:
                    squares[i].type = KING;
                    break;
                default:
                    squares[i].type = PAWN;
                    break;
            }

            if (i < 16) {
                squares[i].color = WHITE;
                whitePieces.push_back({squares[i].type, i});
            } else {
                squares[i].color = BLACK;
                blackPieces.push_back({squares[i].type, i});
            }

        } else {
            squares[i].type = EMPTY;
            squares[i].color = WHITE;
        }
    }

}

ChessBoard::ChessBoard() = default;
