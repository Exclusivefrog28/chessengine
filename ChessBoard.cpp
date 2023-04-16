#include "ChessBoard.h"

void ChessBoard::setStartingPosition() {

    whitePieces = {};
    blackPieces = {};

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
    enPassantSquare = -1;

}

std::ostream &operator<<(std::ostream &os, const ChessBoard &board) {

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Type type = board.squares[i * 8 + j].type;
            Color color = board.squares[i * 8 + j].color;
            os << Util::pieceToString(type, color) << " ";
        }
        os << std::endl;
    }

    return os;
}

void ChessBoard::makeMove(Move move) {

    if (move.flag == ENPASSANT) removePiece(enPassantSquare);
    enPassantSquare = -1;

    if (squares[move.start].type == PAWN || (move.flag >= 1 && move.flag <= 5)) halfMoveClock = 0;
    else halfMoveClock++;

    if (move.player == BLACK) fullMoveClock++;

    if (move.promotionType != EMPTY) {
        removePiece(move.start);
        removePiece(move.end);
        setPiece(move.end, {move.promotionType, move.player});
    } else {
        movePiece(move.start, move.end);

        if (move.flag == DOUBLEPAWNPUSH) {
            enPassantSquare = move.end;
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

    enPassantSquare = -1;

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
            enPassantSquare = passedPawnPosition;
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
    if (piece.color == WHITE) {
        whitePieces.push_back({piece.type, position});
    } else {
        blackPieces.push_back({piece.type, position});
    }
    squares[position] = piece;
}

void ChessBoard::removePiece(short position) {
    if (squares[position].color == WHITE) {
        for (int i = 0; whitePieces.size() > i; i++) {
            if (whitePieces[i].position == position) {
                whitePieces.erase(whitePieces.begin() + i);
                break;
            }
        }
    } else {
        for (int i = 0; blackPieces.size() > i; i++) {
            if (blackPieces[i].position == position) {
                blackPieces.erase(blackPieces.begin() + i);
                break;
            }
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
        if (castlingRights.whiteKingSide || castlingRights.whiteQueenSide) {
            if (move.start == 60) {
                castlingRights.whiteKingSide = false;
                castlingRights.whiteQueenSide = false;
            } else {
                if (castlingRights.whiteKingSide && move.start == 63) castlingRights.whiteKingSide = false;
                if (castlingRights.whiteQueenSide && move.start == 56) castlingRights.whiteQueenSide = false;
            }
        }
        if (castlingRights.blackKingSide && move.end == 7) castlingRights.blackKingSide = false;
        if (castlingRights.blackQueenSide && move.end == 0) castlingRights.blackQueenSide = false;
    } else {
        if (castlingRights.blackKingSide || castlingRights.blackQueenSide) {
            if (move.start == 4) {
                castlingRights.blackKingSide = false;
                castlingRights.blackQueenSide = false;
            } else {
                if (castlingRights.blackKingSide && move.start == 7) castlingRights.blackKingSide = false;
                if (castlingRights.blackQueenSide && move.start == 0) castlingRights.blackQueenSide = false;
            }
        }
        if (castlingRights.whiteKingSide && move.end == 63) castlingRights.whiteKingSide = false;
        if (castlingRights.whiteQueenSide && move.end == 56) castlingRights.whiteQueenSide = false;
    }
}

std::string ChessBoard::fen() {
    std::string fen = "";
    for (int i = 0; i < 8; ++i) {
        int emptyspaces = 0;
        for (int j = 0; j < 8; ++j) {
            Square square = squares[i * 8 + j];
            if (square.type == EMPTY) {
                emptyspaces++;
                if (j == 7) {
                    fen += std::to_string(emptyspaces);
                }
            } else {
                if (emptyspaces > 0) fen += std::to_string(emptyspaces);
                emptyspaces = 0;
                fen += Util::pieceToString(square.type, square.color);
            }
        }
        if (i < 7) { fen += "/"; }
    }
    fen += " ";

    if (sideToMove == WHITE) fen += "w ";
    else fen += "b ";

    std::string fenCastlingRights;

    if(castlingRights.whiteKingSide) fenCastlingRights += "K";
    if(castlingRights.whiteQueenSide) fenCastlingRights += "Q";
    if(castlingRights.blackKingSide) fenCastlingRights += "k";
    if(castlingRights.blackQueenSide) fenCastlingRights += "q";
    if(fenCastlingRights.empty()) fenCastlingRights = "-";

    fen += fenCastlingRights;
    fen += " ";
    fen += (enPassantSquare == -1) ? "-" : Util::positionToString(enPassantSquare);
    fen += " ";
    fen += std::to_string(halfMoveClock);
    fen += " ";
    fen += std::to_string(fullMoveClock);

    return fen;
}

ChessBoard::ChessBoard() = default;
