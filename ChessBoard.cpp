#include "ChessBoard.h"

void ChessBoard::setStartingPosition() {
    setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
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

void ChessBoard::makeMove(const Move &move) {

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
    Move m = move;
    moveHistory.push_back(m);
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

void ChessBoard::movePiece(const short &start, const short &end) {
    if (squares[end].type != EMPTY) removePiece(end);

    Square piece = squares[start];

    squares[end] = squares[start];
    squares[start] = {Pieces::EMPTY, WHITE};

    switch (piece.type) {
        case PAWN: {
            std::vector<short> *pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
            for (int i = 0; i < pawnList->size(); ++i) {
                if ((*pawnList)[i] == start) {
                    (*pawnList)[i] = end;
                    break;
                }
            }
        }
            break;
        case KING: {
            short *kingPosition = (piece.color == WHITE) ? &whiteKing : &blackKing;
            *kingPosition = end;
        }
        default: {
            std::vector<Piece> *pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
            for (int i = 0; i < pieceList->size(); ++i) {
                if ((*pieceList)[i].position == start) {
                    (*pieceList)[i].position = end;
                    break;
                }
            }
        }
            break;
    }
}

void ChessBoard::setPiece(const short &position, const Square piece) {

    switch (piece.type) {
        case PAWN: {
            std::vector<short> *pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
            pawnList->push_back(position);
        }
            break;
        case KING: {
            short *kingPosition = (piece.color == WHITE) ? &whiteKing : &blackKing;
            *kingPosition = position;
        }
        default: {
            std::vector<Piece> *pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
            pieceList->push_back({piece.type, position});
        }
            break;
    }

    squares[position] = piece;
}

void ChessBoard::removePiece(const short &position) {

    Square piece = squares[position];

    if (piece.type == PAWN) {
        std::vector<short> *pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
        for (int i = 0; pawnList->size() > i; i++) {
            if ((*pawnList)[i] == position) {
                pawnList->erase(pawnList->begin() + i);
                break;
            }
        }
    } else {
        std::vector<Piece> *pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
        for (int i = 0; pieceList->size() > i; i++) {
            if ((*pieceList)[i].position == position) {
                pieceList->erase(pieceList->begin() + i);
                break;
            }
        }
    }

    squares[position] = {EMPTY, WHITE};
}

void ChessBoard::updateCastlingRights(const Move &move) {
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
    if (fenPassant < 31) fenPassant += 8; else fenPassant -= 8;

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
    whitePawns = std::vector<short>();
    blackPawns = std::vector<short>();
    whiteKing = -1;
    blackKing = -1;
    moveHistory = std::vector<Move>();
    castlingRightHistory = std::vector<CastlingRights>();
    squares = std::array<Square, 64>();
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
