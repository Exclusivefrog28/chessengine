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
                squares[i].color = BLACK;
                blackPieces.push_back({squares[i].type, i});
            } else {
                squares[i].color = WHITE;
                whitePieces.push_back({squares[i].type, i});
            }

        } else {
            squares[i].type = EMPTY;
            squares[i].color = WHITE;
        }
    }

    sideToMove = WHITE;
    castlingRights = {true, true, true, true};
    enPassantFile = 0;

}

std::ostream &operator<<(std::ostream &os, const ChessBoard &board) {

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int piece = board.squares[i * 8 + j].type;
            std::string code;
            switch (piece) {
                case 1:
                    code = "p";
                    break;
                case 2:
                    code = "N";
                    break;
                case 3:
                    code = "B";
                    break;
                case 4:
                    code = "R";
                    break;
                case 5:
                    code = "Q";
                    break;
                case 6:
                    code = "K";
                    break;
                default:
                    code = "-";
                    break;
            }
            os << code << " ";
        }
        os << std::endl;
    }

    return os;
}

void ChessBoard::makeMove(Move move) {
    enPassantFile = -1;

    if (move.promotionType != EMPTY) {
        removePiece(move.start);
        removePiece(move.end);
        setPiece(move.end, {move.promotionType, move.player});
    } else {
        movePiece(move.start, move.end);

        if (move.flag == ENPASSANT) {
            short passedPawnPosition = (move.player == WHITE) ? move.end + 8 : move.end - 8;
            removePiece(passedPawnPosition);
        } else if (move.flag == DOUBLEPAWNMOVE) {
            short left = move.end - 1;
            short right = move.end + 1;
            if (squares[left].type == PAWN && squares[left].color != move.player
                || squares[right].type == PAWN && squares[right].color != move.player) {
                enPassantFile = enPassantFile = move.start % 8;
            }
        } else if (move.flag == CASTLEKINGSIDE) {
            movePiece(move.end + 1, move.end - 1);
        } else if (move.flag == CASTLEQUEENSIDE) {
            movePiece(move.end - 2, move.end + 1);
        }
    }

    updateCastlingRights(move);
    sideToMove = invertColor(sideToMove);
    moveHistory.push_back(move);
    castlingRightHistory.push_back(castlingRights);
}

void ChessBoard::unMakeMove() {
    Move lastMove = moveHistory[moveHistory.size() - 1];
    castlingRights = castlingRightHistory[castlingRightHistory.size() - 1];
    moveHistory.pop_back();
    castlingRightHistory.pop_back();

    enPassantFile = -1;

    if (lastMove.promotionType != EMPTY) {
        removePiece(lastMove.end);
        setPiece(lastMove.start, {PAWN, lastMove.player});
        if (lastMove.flag > 0) setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), invertColor(lastMove.player)});
    } else {
        movePiece(lastMove.end, lastMove.start);

        if (lastMove.flag > 0 && lastMove.flag < 6) {
            setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), invertColor(lastMove.player)});
        } else if (lastMove.flag == ENPASSANT) {
            short passedPawnPosition = (lastMove.player == WHITE) ? lastMove.end + 8 : lastMove.end - 8;
            setPiece(passedPawnPosition, {PAWN, invertColor(lastMove.player)});
            enPassantFile = lastMove.end % 8;
        } else if (lastMove.flag == CASTLEKINGSIDE) {
            movePiece(lastMove.end - 1, lastMove.end + 1);
        } else if (lastMove.flag == CASTLEQUEENSIDE) {
            movePiece(lastMove.end + 1, lastMove.end - 2);
        }
    }

    sideToMove = invertColor(sideToMove);
}

void ChessBoard::movePiece(short start, short end) {
    if (squares[end].type != EMPTY) removePiece(end);
    setPiece(end, squares[start]);
    removePiece(start);
}

void ChessBoard::setPiece(short position, ChessBoard::Square piece) {
    std::vector<Piece> &pieceList = blackPieces;
    if (piece.color == WHITE) {
        pieceList = whitePieces;
    }
    pieceList.push_back({piece.type, position});
    squares[position] = piece;
}

void ChessBoard::removePiece(short position) {
    std::vector<Piece> &pieceList = blackPieces;
    if (squares[position].color == WHITE) {
        pieceList = whitePieces;
    }
    for (int i = 0; pieceList.size() > i; i++) {
        if (pieceList[i].position == position) {
            pieceList[i] = pieceList[pieceList.size() - 1];
            pieceList.pop_back();
            break;
        }
    }
    squares[position] = {EMPTY, WHITE};
}

Color ChessBoard::invertColor(Color color) {
    if (color == WHITE) return BLACK;
    else return WHITE;
}

void ChessBoard::updateCastlingRights(Move move) {
    if (move.player == WHITE) {
        if (castlingRights.whiteShort || castlingRights.whiteLong) {
            if (move.start == 60) {
                castlingRights.whiteShort = false;
                castlingRights.whiteLong = false;
            } else {
                if (castlingRights.whiteShort && move.start == 63) castlingRights.whiteShort = false;
                if (castlingRights.whiteLong && move.start == 56) castlingRights.whiteLong = false;
            }
        }
        if (castlingRights.blackShort && move.end == 7) castlingRights.blackShort = false;
        if (castlingRights.blackLong && move.end == 0) castlingRights.blackLong = false;
    } else {
        if (castlingRights.blackShort || castlingRights.blackLong) {
            if (move.start == 4) {
                castlingRights.blackShort = false;
                castlingRights.blackLong = false;
            } else {
                if (castlingRights.blackShort && move.start == 7) castlingRights.blackShort = false;
                if (castlingRights.blackLong && move.start == 0) castlingRights.blackLong = false;
            }
        }
        if (castlingRights.whiteShort && move.end == 63) castlingRights.whiteShort = false;
        if (castlingRights.whiteLong && move.end == 56) castlingRights.whiteLong = false;
    }
}

ChessBoard::ChessBoard() = default;
