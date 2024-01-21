#include "MoveGenerator.h"

std::vector<Moves::Move> MoveGenerator::pseudoLegalMoves(const ChessBoard &board) {
    std::vector<Move> moves;

    const std::vector<Piece> *pieceList = (board.sideToMove == WHITE) ? &board.whitePieces : &board.blackPieces;

    for (const Piece &piece: *pieceList) {

        for (short i = 0; i < OFFSETS[piece.type]; ++i) {
            short n = piece.position;
            while (true) {
                n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][i]];
                if (n == -1) break;
                Square target = board.squares[n];
                if (target.type != Pieces::EMPTY) {
                    if (target.color != board.sideToMove)
                        moves.push_back(
                                {piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
                    break;
                }
                moves.push_back({piece.position, n, EMPTY, QUIET, board.sideToMove});
                if (!SLIDE[piece.type]) break;
            }
        }

        if (piece.type == Pieces::KING && !inCheck(board, board.sideToMove)) {
            short kingPosition = piece.position;
            if ((board.castlingRights.whiteKingSide && board.sideToMove == WHITE) ||
                (board.castlingRights.blackKingSide && board.sideToMove == BLACK)) {
                if (!isSquareAttacked(board, kingPosition + 1, board.sideToMove) &&
                    board.squares[kingPosition + 1].type == EMPTY &&
                    !isSquareAttacked(board, kingPosition + 2, board.sideToMove) &&
                    board.squares[kingPosition + 2].type == EMPTY) {
                    moves.push_back({kingPosition, static_cast<short>(kingPosition + 2), EMPTY, CASTLEKINGSIDE,
                                     board.sideToMove});
                }
            }
            if ((board.castlingRights.whiteQueenSide && board.sideToMove == WHITE) ||
                (board.castlingRights.blackQueenSide && board.sideToMove == BLACK)) {

                if (!isSquareAttacked(board, kingPosition - 1, board.sideToMove) &&
                    board.squares[kingPosition - 1].type == EMPTY &&
                    !isSquareAttacked(board, kingPosition - 2, board.sideToMove) &&
                    board.squares[kingPosition - 2].type == EMPTY &&
                    board.squares[kingPosition - 3].type == EMPTY) {
                    moves.push_back({kingPosition, static_cast<short>(kingPosition - 2), EMPTY, CASTLEQUEENSIDE,
                                     board.sideToMove});
                }
            }
        }
    }

    const std::vector<short> *pawnList = (board.sideToMove == WHITE) ? &board.whitePawns : &board.blackPawns;

    for (const short &pawnPosition: *pawnList) {
        short sign = (board.sideToMove == WHITE) ? -1 : 1;

        short pushTarget = pawnPosition + (sign * OFFSET[PAWN][0]);
        if (board.squares[pushTarget].type == EMPTY) {
            if (pushTarget <= 7 || pushTarget >= 56) {
                moves.push_back({pawnPosition, pushTarget, KNIGHT, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, BISHOP, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, ROOK, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, QUEEN, QUIET, board.sideToMove});
            } else {
                moves.push_back({pawnPosition, pushTarget, EMPTY, QUIET, board.sideToMove});
                if (pawnPosition < 16 || pawnPosition >= 48) {
                    short doublePushTarget = pawnPosition + (sign * OFFSET[PAWN][3]);
                    if (board.squares[doublePushTarget].type == EMPTY)
                        moves.push_back(
                                {pawnPosition, doublePushTarget, EMPTY, DOUBLEPAWNPUSH, board.sideToMove});
                }
            }
        }
        for (short i = 1; i < 3; ++i) {
            short n = MAILBOX[MAILBOX64[pawnPosition] + (sign * OFFSET[PAWN][i])];
            if (n == -1) continue;
            Square target = board.squares[n];
            if (target.type != EMPTY && target.color != board.sideToMove) {
                if (n <= 7 || n >= 56) {
                    moves.push_back(
                            {pawnPosition, n, KNIGHT, static_cast<MoveFlag>(target.type), board.sideToMove});
                    moves.push_back(
                            {pawnPosition, n, BISHOP, static_cast<MoveFlag>(target.type), board.sideToMove});
                    moves.push_back(
                            {pawnPosition, n, ROOK, static_cast<MoveFlag>(target.type), board.sideToMove});
                    moves.push_back(
                            {pawnPosition, n, QUEEN, static_cast<MoveFlag>(target.type), board.sideToMove});
                } else
                    moves.push_back(
                            {pawnPosition, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
            }
            if (board.enPassantSquare != -1) {
                short enPassantTarget = n - (sign * OFFSET[PAWN][0]);
                if (enPassantTarget == board.enPassantSquare)
                    moves.push_back({pawnPosition, n, EMPTY, ENPASSANT, board.sideToMove});
            }
        }
    }

    return moves;
}

std::vector<Moves::Move> MoveGenerator::tacticalMoves(const ChessBoard &board) {

    std::vector<Move> moves;

    const std::vector<Piece> *pieceList = (board.sideToMove == WHITE) ? &board.whitePieces : &board.blackPieces;

    for (const Piece &piece: *pieceList) {

        for (short i = 0; i < OFFSETS[piece.type]; ++i) {
            short n = piece.position;
            while (true) {
                n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][i]];
                if (n == -1) break;
                Square target = board.squares[n];
                if (target.type != Pieces::EMPTY) {
                    if (target.color != board.sideToMove)
                        moves.push_back(
                                {piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
                    break;
                }
                if (!SLIDE[piece.type]) break;
            }
        }
    }

    const std::vector<short> *pawnList = (board.sideToMove == WHITE) ? &board.whitePawns : &board.blackPawns;

    for (const short &pawnPosition: *pawnList) {
        short sign = (board.sideToMove == WHITE) ? -1 : 1;

        short pushTarget = pawnPosition + (sign * OFFSET[PAWN][0]);
        if (board.squares[pushTarget].type == EMPTY) {
            if (pushTarget <= 7 || pushTarget >= 56) {
                moves.push_back({pawnPosition, pushTarget, KNIGHT, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, BISHOP, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, ROOK, QUIET, board.sideToMove});
                moves.push_back({pawnPosition, pushTarget, QUEEN, QUIET, board.sideToMove});
            }
            for (short i = 1; i < 3; ++i) {
                short n = MAILBOX[MAILBOX64[pawnPosition] + (sign * OFFSET[PAWN][i])];
                if (n == -1) continue;
                Square target = board.squares[n];
                if (target.type != EMPTY && target.color != board.sideToMove) {
                    if (n <= 7 || n >= 56) {
                        moves.push_back(
                                {pawnPosition, n, KNIGHT, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back(
                                {pawnPosition, n, BISHOP, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back(
                                {pawnPosition, n, ROOK, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back(
                                {pawnPosition, n, QUEEN, static_cast<MoveFlag>(target.type), board.sideToMove});
                    } else
                        moves.push_back(
                                {pawnPosition, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
                }
                if (board.enPassantSquare != -1) {
                    short enPassantTarget = n - (sign * OFFSET[PAWN][0]);
                    if (enPassantTarget == board.enPassantSquare)
                        moves.push_back({pawnPosition, n, EMPTY, ENPASSANT, board.sideToMove});
                }
            }
        }
    }

    return moves;
}


bool MoveGenerator::isSquareAttacked(const ChessBoard &board, short square, Color color) {
    short sign = (color == WHITE) ? -1 : 1;

    //PAWNS
    for (short i = 1; i < 3; ++i) {
        short n = MAILBOX[MAILBOX64[square] + (sign * OFFSET[PAWN][i])];
        if (n != -1) {
            if (board.squares[n].color != color && board.squares[n].type == PAWN) return true;
        }
    }
    //KNIGHTS
    for (short i = 0; i < OFFSETS[KNIGHT]; ++i) {
        short n = MAILBOX[MAILBOX64[square] + OFFSET[KNIGHT][i]];
        if (n != -1) {
            if (board.squares[n].color != color && board.squares[n].type == KNIGHT) return true;
        }
    }
    //REST
    for (short i = 0; i < OFFSETS[QUEEN]; ++i) {
        short n = square;
        short offset = OFFSET[QUEEN][i];
        bool sliding = false;
        while (true) {
            n = MAILBOX[MAILBOX64[n] + offset];
            if (n == -1) break;
            Square target = board.squares[n];
            if (target.type != Pieces::EMPTY) {
                if (target.color != color && (SLIDE[target.type] || !sliding) && target.type != PAWN &&
                    target.type != KNIGHT) {
                    const short *offsetPtr = std::find(OFFSET[target.type], OFFSET[target.type] + OFFSETS[target.type],
                                                       offset);
                    if (offsetPtr != OFFSET[target.type] + OFFSETS[target.type]) return true;
                }
                break;
            }
            sliding = true;
        }
    }

    return false;
}

bool MoveGenerator::inCheck(const ChessBoard &board, Color color) {
    short kingPosition = (color == Pieces::WHITE) ? board.whiteKing : board.blackKing;

    return MoveGenerator::isSquareAttacked(board, kingPosition, color);
}

unsigned long long MoveGenerator::perft(int depth, ChessBoard &board) {
    if (depth == 0) return 1ULL;
    unsigned long long nodes = 0ULL;

    std::vector<Move> moves = pseudoLegalMoves(board);

    for (const Move move: moves) {
        board.makeMove(move);
        if (!inCheck(board, Pieces::invertColor(board.sideToMove))) {
            unsigned long long childNodes = perft(depth - 1, board);
//            if(depth == 6){
//                printf("Move: %s-%s %d\n", Util::positionToString(move.start).c_str(), Util::positionToString(move.end).c_str(), move.flag);
//                printf("Nodes: %llu\n", childNodes);
//            }
            nodes += childNodes;
        }
        board.unMakeMove();
    }

    return nodes;
}

bool MoveGenerator::isLegalMove(ChessBoard &board, Moves::Move move) {
    bool isLegal = false;
    board.makeMove(move);
    if (!inCheck(board, Pieces::invertColor(board.sideToMove))) isLegal = true;
    board.unMakeMove();
    return isLegal;
}
