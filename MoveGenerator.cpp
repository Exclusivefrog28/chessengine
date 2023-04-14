#include "MoveGenerator.h"

std::vector<Moves::Move> MoveGenerator::pseudoLegalMoves(ChessBoard board) {
    std::vector<Move> moves;

    std::vector<ChessBoard::Piece> *pieceList;
    if (board.sideToMove == WHITE) pieceList = &board.whitePieces;
    else pieceList = &board.blackPieces;

    for (const ChessBoard::Piece &piece: *pieceList) {
        if (piece.type != Piece::PAWN) {
            for (short j = 0; j < OFFSETS[piece.type]; ++j) {
                short n = piece.position;
                while (true) {
                    n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][j]];
                    if (n == -1) break;
                    ChessBoard::Square target = board.squares[n];
                    if (target.type != Piece::EMPTY) {
                        if (target.color != board.sideToMove)
                            moves.push_back(
                                    {piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
                        break;
                    }
                    moves.push_back({piece.position, n, EMPTY, QUIET, board.sideToMove});
                    if (!SLIDE[piece.type]) break;
                }
            }
        } else {
            short sign = (board.sideToMove == WHITE) ? -1 : 1;

            short pushTarget = piece.position + (sign * OFFSET[PAWN][0]);
            if (board.squares[pushTarget].type == EMPTY) {
                if (pushTarget <= 7 || pushTarget >= 56) {
                    moves.push_back({piece.position, pushTarget, KNIGHT, QUIET, board.sideToMove});
                    moves.push_back({piece.position, pushTarget, BISHOP, QUIET, board.sideToMove});
                    moves.push_back({piece.position, pushTarget, ROOK, QUIET, board.sideToMove});
                    moves.push_back({piece.position, pushTarget, QUEEN, QUIET, board.sideToMove});
                } else {
                    moves.push_back({piece.position, pushTarget, EMPTY, QUIET, board.sideToMove});
                    if (piece.position < 16 || piece.position >= 48) {
                        short doublePushTarget = piece.position + (sign * OFFSET[PAWN][3]);
                        if (board.squares[pushTarget].type == EMPTY)
                            moves.push_back({piece.position, doublePushTarget, EMPTY, DOUBLEPAWNPUSH, board.sideToMove});
                    }
                }
            }
            for (int i = 2; i < 4; ++i) {
                short n = MAILBOX[MAILBOX64[piece.position] + OFFSET[piece.type][i]];
                if (n == -1) break;
                ChessBoard::Square target = board.squares[n];
                if(target.type != EMPTY && target.color != board.sideToMove){
                    if (n <= 7 || n >= 56) {
                        moves.push_back({piece.position, pushTarget, KNIGHT, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back({piece.position, pushTarget, BISHOP, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back({piece.position, pushTarget, ROOK, static_cast<MoveFlag>(target.type), board.sideToMove});
                        moves.push_back({piece.position, pushTarget, QUEEN, static_cast<MoveFlag>(target.type), board.sideToMove});
                    } else moves.push_back({piece.position, pushTarget, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
                }
                if(target.type == EMPTY && board.enPassantSquare != -1){
                    short enPassantTarget = n - (sign * OFFSET[PAWN][0]);
                    if(enPassantTarget == board.enPassantSquare) moves.push_back({piece.position, enPassantTarget, EMPTY, ENPASSANT, board.sideToMove});
                }
            }
        }
    }

    return moves;
}
