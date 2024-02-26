#include "MoveGenerator.h"

#include <cmath>
#include <thread>

#define NUM_OF_THREADS 6

void MoveGenerator::addMove(const Move&move) {
	std::lock_guard<std::mutex> lock(movesMutex);
	moves.push_back(move);
}

void MoveGenerator::generateMovesForPieces(const std::vector<Piece>&pieces) {
	for (const Piece piece: pieces) {
		for (int i = 0; i < OFFSETS[piece.type]; ++i) {
			int_fast8_t n = piece.position;
			while (true) {
				n = MAILBOX[MAILBOX64[n] + OFFSET[piece.type][i]];
				if (n == -1) break;
				Square target = board.squares[n];
				if (target.type != EMPTY) {
					if (target.color != board.sideToMove) {
						addMove({piece.position, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
					}
					break;
				}
				addMove({piece.position, n, EMPTY, QUIET, board.sideToMove});
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
					addMove({
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
					addMove({
						kingPosition, static_cast<int_fast8_t>(kingPosition - 2), EMPTY, CASTLEQUEENSIDE,
						board.sideToMove
					});
				}
			}
		}
	}
}

void MoveGenerator::generateMovesForPawns(const std::vector<int_fast8_t>&pawnPositions) {
	for (const int_fast8_t pawnPosition: pawnPositions) {
		const int_fast8_t sign = (board.sideToMove == WHITE) ? -1 : 1;

		const int_fast8_t pushTarget = pawnPosition + (sign * OFFSET[PAWN][0]);
		if (board.squares[pushTarget].type == EMPTY) {
			if (pushTarget <= 7 || pushTarget >= 56) {
				addMove({pawnPosition, pushTarget, KNIGHT, QUIET, board.sideToMove});
				addMove({pawnPosition, pushTarget, BISHOP, QUIET, board.sideToMove});
				addMove({pawnPosition, pushTarget, ROOK, QUIET, board.sideToMove});
				addMove({pawnPosition, pushTarget, QUEEN, QUIET, board.sideToMove});
			}
			else {
				addMove({pawnPosition, pushTarget, EMPTY, QUIET, board.sideToMove});
				if (pawnPosition < 16 || pawnPosition >= 48) {
					const int_fast8_t doublePushTarget = pawnPosition + (sign * OFFSET[PAWN][3]);
					if (board.squares[doublePushTarget].type == EMPTY) {
						addMove({pawnPosition, doublePushTarget, EMPTY, DOUBLEPAWNPUSH, board.sideToMove});
					}
				}
			}
		}
		for (int i = 1; i < 3; ++i) {
			const int_fast8_t n = MAILBOX[MAILBOX64[pawnPosition] + (sign * OFFSET[PAWN][i])];
			if (n == -1) continue;
			Square target = board.squares[n];
			if (target.type != EMPTY && target.color != board.sideToMove) {
				if (n <= 7 || n >= 56) {
					addMove({pawnPosition, n, KNIGHT, static_cast<MoveFlag>(target.type), board.sideToMove});
					addMove({pawnPosition, n, BISHOP, static_cast<MoveFlag>(target.type), board.sideToMove});
					addMove({pawnPosition, n, ROOK, static_cast<MoveFlag>(target.type), board.sideToMove});
					addMove({pawnPosition, n, QUEEN, static_cast<MoveFlag>(target.type), board.sideToMove});
				}
				else addMove({pawnPosition, n, EMPTY, static_cast<MoveFlag>(target.type), board.sideToMove});
			}
			if (board.enPassantSquare != -1) {
				const int_fast8_t enPassantTarget = n - (sign * OFFSET[PAWN][0]);
				if (enPassantTarget == board.enPassantSquare) {
					addMove({pawnPosition, n, EMPTY, ENPASSANT, board.sideToMove});
				}
			}
		}
	}
}

std::vector<Move> MoveGenerator::pseudoLegalMoves(const ChessBoard&board) {
	MoveGenerator generator;
	generator.board = board;

	std::array<std::thread,NUM_OF_THREADS> threads;

	const std::vector<Piece>* pieceList = (board.sideToMove == WHITE) ? &board.whitePieces : &board.blackPieces;
	const std::vector<int_fast8_t>* pawnList = (board.sideToMove == WHITE) ? &board.whitePawns : &board.blackPawns;

	const int pieceListLength = pieceList->size();
	const int pawnListLength = pawnList->size();
	const int totalLength = pieceListLength + pawnListLength;

	const int tasksPerThread = std::ceil(totalLength / static_cast<float>(NUM_OF_THREADS));
	const int numPieceThreads = std::ceil(pieceListLength / static_cast<float>(tasksPerThread));
	const int numPawnThreads = NUM_OF_THREADS - tasksPerThread;

	std::vector<std::vector<Piece>> pieceTasks;
	for (int i = 0; i < pieceListLength; i += tasksPerThread) {
		const int last = std::min(pieceListLength, i + tasksPerThread);
		std::vector<Piece> vec;
		vec.reserve(last - i);
		move(pieceList->begin() + i, pieceList->begin() + last, back_inserter(vec));
		pieceTasks.push_back(vec);
	}

	for (int i = 0; i < numPieceThreads; ++i) {
		threads[i] = std::thread(&MoveGenerator::generateMovesForPieces, &generator, std::cref(pieceTasks[i]));
	}

	std::vector<std::vector<int_fast8_t>> pawnTasks;
	for (int i = 0; i < pawnListLength; i += tasksPerThread) {
		const int last = std::min(pawnListLength, i + tasksPerThread);
		std::vector<int_fast8_t> vec;
		vec.reserve(last - i);
		move(pawnList->begin() + i, pawnList->begin() + last, back_inserter(vec));
		pawnTasks.push_back(vec);
	}

	for (int i = numPieceThreads; i < NUM_OF_THREADS; ++i) {
		threads[i] = std::thread(&MoveGenerator::generateMovesForPawns, &generator, std::cref(pawnTasks[i - numPieceThreads]));
	}

	for (std::thread&thread: threads) {
		thread.join();
	}

	return generator.moves;
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


bool MoveGenerator::isSquareAttacked(const ChessBoard&board, int_fast8_t square, Color color) {
	const int_fast8_t sign = (color == WHITE) ? -1 : 1;

	//PAWNS
	for (int i = 1; i < 3; ++i) {
		const int_fast8_t n = MAILBOX[MAILBOX64[square] + (sign * OFFSET[PAWN][i])];
		if (n != -1) {
			if (board.squares[n].color != color && board.squares[n].type == PAWN) return true;
		}
	}
	//KNIGHTS
	for (int i = 0; i < OFFSETS[KNIGHT]; ++i) {
		const int_fast8_t n = MAILBOX[MAILBOX64[square] + OFFSET[KNIGHT][i]];
		if (n != -1) {
			if (board.squares[n].color != color && board.squares[n].type == KNIGHT) return true;
		}
	}
	//REST
	for (int i = 0; i < OFFSETS[QUEEN]; ++i) {
		int_fast8_t n = square;
		int_fast8_t offset = OFFSET[QUEEN][i];
		bool sliding = false;
		while (true) {
			n = MAILBOX[MAILBOX64[n] + offset];
			if (n == -1) break;
			const Square target = board.squares[n];
			if (target.type != EMPTY) {
				if (target.color != color && (SLIDE[target.type] || !sliding) && target.type != PAWN &&
				    target.type != KNIGHT) {
					const int_fast8_t* offsetPtr = std::find(OFFSET[target.type], OFFSET[target.type] + OFFSETS[target.type],
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

bool MoveGenerator::inCheck(const ChessBoard&board, Color color) {
	const int_fast8_t kingPosition = (color == WHITE) ? board.whiteKing : board.blackKing;

	return isSquareAttacked(board, kingPosition, color);
}

unsigned long long MoveGenerator::perft(int depth, ChessBoard&board) {
	if (depth == 0) return 1ULL;
	unsigned long long nodes = 0ULL;

	const std::vector<Move> moves = pseudoLegalMoves(board);

	for (const Move move: moves) {
		board.makeMove(move);
		if (!inCheck(board, invertColor(board.sideToMove))) {
			const unsigned long long childNodes = perft(depth - 1, board);
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

bool MoveGenerator::isLegalMove(ChessBoard&board, Move move) {
	bool isLegal = false;
	board.makeMove(move);
	if (!inCheck(board, invertColor(board.sideToMove))) isLegal = true;
	board.unMakeMove();
	return isLegal;
}
