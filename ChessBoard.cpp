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

    if (move.flag < 6) {
        movePiece(move.start, move.end);
    }

    sideToMove = invertColor(sideToMove);
}

void ChessBoard::unMakeMove() {
    Move lastMove = history[history.size() - 1];
    history.pop_back();


    if (lastMove.flag < 6) {
        movePiece(lastMove.end, lastMove.start);

        if (lastMove.flag > 0) {
            setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), invertColor(lastMove.player)});
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

ChessBoard::ChessBoard() = default;
