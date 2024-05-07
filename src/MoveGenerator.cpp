#include "MoveGenerator.h"

std::vector<Move> MoveGenerator::pseudoLegalMoves(const ChessBoard&board) {
	std::vector<Move> moves;

	const std::vector<Piece>* pieceList = (board.sideToMove == WHITE) ? &board.whitePieces : &board.blackPieces;

	for (const Piece&piece: *pieceList) {
		for (int i = 0; i < OFFSETS[piece.type]; ++i) {
			int_fast8_t n = piece.position;
			while (true) {
				n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][i]];
				if (n == -1) break;
				Square target = board.squares[n];
				if (target.type != EMPTY) {
					if (target.color != board.sideToMove)
						moves.push_back(
							{piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
					break;
				}
				moves.push_back({piece.position, n, EMPTY, QUIET, board.sideToMove});
				if (!SLIDE[piece.type]) break;
			}
		}

		if (piece.type == KING && !inCheck(board, board.sideToMove)) {
			const int_fast8_t kingPosition = piece.position;
			if ((board.castlingRights.whiteKingSide && board.sideToMove == WHITE) ||
			    (board.castlingRights.blackKingSide && board.sideToMove == BLACK)) {
				if (!isSquareAttacked(board, kingPosition + 1, board.sideToMove) &&
				    board.squares[kingPosition + 1].type == EMPTY &&
				    !isSquareAttacked(board, kingPosition + 2, board.sideToMove) &&
				    board.squares[kingPosition + 2].type == EMPTY) {
					moves.push_back({
						kingPosition, static_cast<int_fast8_t>(kingPosition + 2), EMPTY, CASTLEKINGSIDE,
						board.sideToMove
					});
				}
			}
			if ((board.castlingRights.whiteQueenSide && board.sideToMove == WHITE) ||
			    (board.castlingRights.blackQueenSide && board.sideToMove == BLACK)) {
				if (!isSquareAttacked(board, kingPosition - 1, board.sideToMove) &&
				    board.squares[kingPosition - 1].type == EMPTY &&
				    !isSquareAttacked(board, kingPosition - 2, board.sideToMove) &&
				    board.squares[kingPosition - 2].type == EMPTY &&
				    board.squares[kingPosition - 3].type == EMPTY) {
					moves.push_back({
						kingPosition, static_cast<int_fast8_t>(kingPosition - 2), EMPTY, CASTLEQUEENSIDE,
						board.sideToMove
					});
				}
			}
		}
	}

	const std::vector<int_fast8_t>* pawnList = (board.sideToMove == WHITE) ? &board.whitePawns : &board.blackPawns;

	for (const int_fast8_t&pawnPosition: *pawnList) {
		const int_fast8_t sign = (board.sideToMove == WHITE) ? -1 : 1;

		const int_fast8_t pushTarget = pawnPosition + (sign * OFFSET[PAWN][0]);
		if (board.squares[pushTarget].type == EMPTY) {
			if (pushTarget <= 7 || pushTarget >= 56) {
				moves.push_back({pawnPosition, pushTarget, KNIGHT, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, BISHOP, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, ROOK, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, QUEEN, QUIET, board.sideToMove});
			}
			else {
				moves.push_back({pawnPosition, pushTarget, EMPTY, QUIET, board.sideToMove});
				if (pawnPosition < 16 || pawnPosition >= 48) {
					const int_fast8_t doublePushTarget = pawnPosition + (sign * OFFSET[PAWN][3]);
					if (board.squares[doublePushTarget].type == EMPTY)
						moves.push_back(
							{pawnPosition, doublePushTarget, EMPTY, DOUBLEPAWNPUSH, board.sideToMove});
				}
			}
		}
		for (int i = 1; i < 3; ++i) {
			const int_fast8_t n = MAILBOX[MAILBOX64[pawnPosition] + (sign * OFFSET[PAWN][i])];
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
				}
				else
					moves.push_back(
						{pawnPosition, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
			}
			if (board.enPassantSquare != -1) {
				const int_fast8_t enPassantTarget = n - (sign * OFFSET[PAWN][0]);
				if (enPassantTarget == board.enPassantSquare)
					moves.push_back({pawnPosition, n, EMPTY, ENPASSANT, board.sideToMove});
			}
		}
	}

	return moves;
}

std::vector<Move> MoveGenerator::tacticalMoves(const ChessBoard&board) {
	std::vector<Move> moves;

	const std::vector<Piece>* pieceList = (board.sideToMove == WHITE) ? &board.whitePieces : &board.blackPieces;

	for (const Piece&piece: *pieceList) {
		for (int i = 0; i < OFFSETS[piece.type]; ++i) {
			int_fast8_t n = piece.position;
			while (true) {
				n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][i]];
				if (n == -1) break;
				Square target = board.squares[n];
				if (target.type != EMPTY) {
					if (target.color != board.sideToMove)
						moves.push_back(
							{piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
					break;
				}
				if (!SLIDE[piece.type]) break;
			}
		}
	}

	const std::vector<int_fast8_t>* pawnList = (board.sideToMove == WHITE) ? &board.whitePawns : &board.blackPawns;

	for (const int_fast8_t&pawnPosition: *pawnList) {
		const int_fast8_t sign = (board.sideToMove == WHITE) ? -1 : 1;

		const int_fast8_t pushTarget = pawnPosition + (sign * OFFSET[PAWN][0]);
		if (board.squares[pushTarget].type == EMPTY) {
			if (pushTarget <= 7 || pushTarget >= 56) {
				moves.push_back({pawnPosition, pushTarget, KNIGHT, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, BISHOP, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, ROOK, QUIET, board.sideToMove});
				moves.push_back({pawnPosition, pushTarget, QUEEN, QUIET, board.sideToMove});
			}
			for (int i = 1; i < 3; ++i) {
				const int_fast8_t n = MAILBOX[MAILBOX64[pawnPosition] + (sign * OFFSET[PAWN][i])];
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
					}
					else
						moves.push_back(
							{pawnPosition, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
				}
				if (board.enPassantSquare != -1) {
					const int_fast8_t enPassantTarget = n - (sign * OFFSET[PAWN][0]);
					if (enPassantTarget == board.enPassantSquare)
						moves.push_back({pawnPosition, n, EMPTY, ENPASSANT, board.sideToMove});
				}
			}
		}
	}

	return moves;
}


bool MoveGenerator::isSquareAttacked(const ChessBoard&board, const int_fast8_t square, const Color color) {
	const int_fast8_t sign = (color == WHITE) ? -1 : 1;

	//PAWNS
	for (int i = 1; i < 3; ++i) {
		const int_fast8_t n = MAILBOX[MAILBOX64[square] + (sign * OFFSET[PAWN][i])];
		if (n != -1) {
			if (board.squares[n].type == PAWN && board.squares[n].color != color) return true;
		}
	}
	//KNIGHTS
	for (int i = 0; i < OFFSETS[KNIGHT]; ++i) {
		const int_fast8_t n = MAILBOX[MAILBOX64[square] + OFFSET[KNIGHT][i]];
		if (n != -1) {
			if (board.squares[n].type == KNIGHT && board.squares[n].color != color) return true;
		}
	}
	//REST
	for (int i = 0; i < OFFSETS[QUEEN]; ++i) {
		int_fast8_t n = square;
		const int_fast8_t offset = OFFSET[QUEEN][i];
		bool sliding = false;
		while (true) {
			n = MAILBOX[MAILBOX64[n] + offset];
			if (n == -1) break;
			const Square target = board.squares[n];
			if (target.type != EMPTY) {
				if (target.color != color && (SLIDE[target.type] || !sliding) && target.type != PAWN &&
				    target.type != KNIGHT) {
					if (target.type == QUEEN || target.type == KING) return true;
					if (i % 2 == 0) {if (target.type == BISHOP) return true;}
					else {if (target.type == ROOK) return true;}
				}
				break;
			}
			sliding = true;
		}
	}

	return false;
}

bool MoveGenerator::inCheck(const ChessBoard&board, Color color) {
	const int_fast8_t kingPosition = (color == WHITE) ? board.whiteKing : board.blackKing;

	return isSquareAttacked(board, kingPosition, color);
}

uint64_t MoveGenerator::perft(int depth, ChessBoard&board) {
	if (depth == 0) return 1ULL;
    uint64_t nodes = 0ULL;

	const std::vector<Move> moves = pseudoLegalMoves(board);

	for (const Move move: moves) {
		board.makeMove(move);
		if (!inCheck(board, invertColor(board.sideToMove))) {
			const uint64_t childNodes = perft(depth - 1, board);
			nodes += childNodes;
		}
		board.unMakeMove();
	}

	return nodes;
}

bool MoveGenerator::isLegalMove(ChessBoard&board, Move move) {
	bool isLegal = false;
	board.makeMove(move);
	if (!inCheck(board, invertColor(board.sideToMove))) isLegal = true;
	board.unMakeMove();
	return isLegal;
}
