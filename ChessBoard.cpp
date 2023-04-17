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

    enPassantHistory.push_back(enPassantSquare);
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

    sideToMove = Pieces::invertColor(sideToMove);
    moveHistory.push_back(move);
    castlingRightHistory.push_back(castlingRights);
    updateCastlingRights(move);
}

void ChessBoard::unMakeMove() {
    Move lastMove = moveHistory[moveHistory.size() - 1];
    castlingRights = castlingRightHistory[castlingRightHistory.size() - 1];
    enPassantSquare = enPassantHistory[enPassantHistory.size() - 1];

    moveHistory.pop_back();
    castlingRightHistory.pop_back();
    enPassantHistory.pop_back();

    if (lastMove.promotionType != EMPTY) {
        removePiece(lastMove.end);
        setPiece(lastMove.start, {PAWN, lastMove.player});
        if (lastMove.flag > 0)
            setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), Pieces::invertColor(lastMove.player)});
    } else {
        movePiece(lastMove.end, lastMove.start);

        if (lastMove.flag > 0 && lastMove.flag < 6) {
            setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), Pieces::invertColor(lastMove.player)});
        } else if (lastMove.flag == ENPASSANT) {
            short passedPawnPosition = (lastMove.player == WHITE) ? lastMove.end + 8 : lastMove.end - 8;
            setPiece(passedPawnPosition, {PAWN, Pieces::invertColor(lastMove.player)});
        } else if (lastMove.flag == CASTLEKINGSIDE) {
            movePiece(lastMove.end - 1, lastMove.end + 1);
        } else if (lastMove.flag == CASTLEQUEENSIDE) {
            movePiece(lastMove.end + 1, lastMove.end - 2);
        }
    }

    sideToMove = Pieces::invertColor(sideToMove);
}

void ChessBoard::movePiece(short start, short end) {
    if (squares[end].type != EMPTY) removePiece(end);
    std::vector<Piece> *pieceList = (squares[start].color == WHITE) ? &whitePieces : &blackPieces;

    squares[end] = squares[start];
    squares[start] =  {Pieces::EMPTY, WHITE};


    for (int i = 0; i < (*pieceList).size(); ++i) {
        if ((*pieceList)[i].position == start) {
            (*pieceList)[i].position = end;
            break;
        }
    }
}

void ChessBoard::setPiece(short position, Square piece) {
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

    if (castlingRights.whiteKingSide) fenCastlingRights += "K";
    if (castlingRights.whiteQueenSide) fenCastlingRights += "Q";
    if (castlingRights.blackKingSide) fenCastlingRights += "k";
    if (castlingRights.blackQueenSide) fenCastlingRights += "q";
    if (fenCastlingRights.empty()) fenCastlingRights = "-";

    fen += fenCastlingRights;
    fen += " ";

    short fenPassant = enPassantSquare;
    if(fenPassant < 31) fenPassant += 8; else fenPassant -= 8;

    fen += (enPassantSquare == -1) ? "-" : Util::positionToString(fenPassant);
    fen += " ";
    fen += std::to_string(halfMoveClock);
    fen += " ";
    fen += std::to_string(fullMoveClock);

    return fen;
}

void ChessBoard::setPosition(std::string fen) {
    short position = 0;
    int index = 0;

    whitePieces = std::vector<Piece>();
    blackPieces = std::vector<Piece>();
    moveHistory = std::vector<Move>();
    castlingRightHistory = std::vector<CastlingRights>();
    squares = std::array<Square,64>();
    enPassantSquare = -1;


    while (fen[index] != ' ') {

        if (fen[index] == '/') {
            index++;
            continue;
        }
        if (fen[index] >= '1' && fen[index] <= '9') {
            for (int j = 0; j < fen[index] - '0'; ++j) {
                squares[position] = {EMPTY, WHITE};
                position++;
            }
        } else {
            setPiece(position, Util::charToPiece(fen[index]));
            position++;
        }
        index++;
    }

    index++;
    sideToMove = (fen[index] == 'w') ? WHITE : BLACK;
    index += 2;

    castlingRights.blackKingSide = false;
    castlingRights.blackQueenSide = false;
    castlingRights.whiteKingSide = false;
    castlingRights.whiteQueenSide = false;

    if (fen[index] != '-') {
        for (; index < index + 4; ++index) {
            switch (fen[index]) {
                case 'K':
                    castlingRights.whiteKingSide = true;
                    break;
                case 'Q':
                    castlingRights.whiteQueenSide = true;
                    break;
                case 'k':
                    castlingRights.blackKingSide = true;
                    break;
                case 'q':
                    castlingRights.blackQueenSide = true;
                    break;
                default:
                    break;
            }
            if (fen[index] == ' ') break;
        }
    } else index++;
    index++;

    if (fen[index] != '-') {
        short fenPassant = Util::stringToPosition(fen.substr(index, 2));
        if (fenPassant < 32) fenPassant += 8; else fenPassant -= 8;
        enPassantSquare = fenPassant;
        index++;
    }
    index += 2;

    int halfMoveLength = 0;
    while (fen[index + halfMoveLength] != ' ') halfMoveLength++;
    halfMoveClock = std::stoi(fen.substr(index, halfMoveLength));
    index += halfMoveLength + 1;

    int fullMoveLength = 0;
    while (index + fullMoveLength != fen.length() && fen[index + fullMoveLength] != ' ') fullMoveLength++;
    fullMoveClock = std::stoi(fen.substr(index, fullMoveLength));
}

ChessBoard::ChessBoard() = default;
