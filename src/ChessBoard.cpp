#include "ChessBoard.h"
#include "HashCodes.h"

void ChessBoard::setStartingPosition() {
	setPosition("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

std::ostream& operator<<(std::ostream&os, const ChessBoard&board) {
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			const Type type = board.squares[i * 8 + j].type;
			const Color color = board.squares[i * 8 + j].color;
			os << Util::pieceToString(type, color) << " ";
		}
		os << std::endl;
	}

	return os;
}

void ChessBoard::makeMove(const Move&move) {
	enPassantHistory.push_back(enPassantSquare);
	castlingRightHistory.push_back(castlingRights);
	halfMoveClockHistory.push_back(halfMoveClock);
	positionHistory.push_back(hashCode);

	if (move.flag == ENPASSANT) {
		removePiece(enPassantSquare);
	}
	if (enPassantSquare != -1) {
		hashCode ^= hashCodes.enPassantFileCode[enPassantSquare % 8];
		enPassantSquare = -1;
	}

	if (squares[move.start].type == PAWN || (move.flag > 0)) {
		halfMoveClock = 0;
		irreversibleIndices.push_back(positionHistory.size() - 1);
	}
	else halfMoveClock++;

	if (move.player == BLACK) fullMoveClock++;

	if (move.promotionType != EMPTY) {
		removePiece(move.start);
		if (squares[move.end].type != EMPTY) removePiece(move.end);
		setPiece(move.end, {move.promotionType, move.player});
	}
	else {
		movePiece(move.start, move.end);

		if (move.flag == DOUBLEPAWNPUSH) {
			enPassantSquare = move.end;
			hashCode ^= hashCodes.enPassantFileCode[move.end % 8];
		}
		else if (move.flag == CASTLEKINGSIDE) {
			movePiece(move.end + 1, move.end - 1);
		}
		else if (move.flag == CASTLEQUEENSIDE) {
			movePiece(move.end - 2, move.end + 1);
		}
	}

	sideToMove = invertColor(sideToMove);
	hashCode ^= hashCodes.blackToMoveCode;

	const Move m = move;
	moveHistory.push_back(m);


	updateCastlingRights(move);

	const auto [blackKingSide, blackQueenSide, whiteKingSide, whiteQueenSide] = castlingRightHistory.back();

	const uint64_t previousHash = hashCodes.castlingRightCodes[blackKingSide * 8 +
	                                                           blackQueenSide * 4 +
	                                                           whiteKingSide * 2 +
	                                                           whiteQueenSide];

	const uint64_t newHash = hashCodes.castlingRightCodes[castlingRights.blackKingSide * 8 + castlingRights.blackQueenSide * 4 +
	                                                      castlingRights.whiteKingSide * 2 + castlingRights.whiteQueenSide];
	if (previousHash != newHash) {
		hashCode ^= previousHash;
		hashCode ^= newHash;
		if (irreversibleIndices.empty() || irreversibleIndices.back() != positionHistory.size() - 1)
			irreversibleIndices.push_back(positionHistory.size() - 1);
	}
}

void ChessBoard::unMakeMove() {
	if (moveHistory.empty()) return;

	if (enPassantSquare != -1) hashCode ^= hashCodes.enPassantFileCode[enPassantSquare % 8];
	enPassantSquare = enPassantHistory.back();
	if (enPassantSquare != -1) hashCode ^= hashCodes.enPassantFileCode[enPassantSquare % 8];

	Move lastMove = moveHistory.back();

	const CastlingRights prevCastlingRights = castlingRightHistory.back();

	if (castlingRights != prevCastlingRights) {
		hashCode ^= hashCodes.castlingRightCodes[castlingRights.blackKingSide * 8 + castlingRights.blackQueenSide * 4 +
		                                         castlingRights.whiteKingSide * 2 + castlingRights.whiteQueenSide];
		hashCode ^= hashCodes.castlingRightCodes[prevCastlingRights.blackKingSide * 8 +
		                                         prevCastlingRights.blackQueenSide * 4 +
		                                         prevCastlingRights.whiteKingSide * 2 +
		                                         prevCastlingRights.whiteQueenSide];
		castlingRights = prevCastlingRights;
	}

	halfMoveClock = halfMoveClockHistory.back();

	moveHistory.pop_back();
	castlingRightHistory.pop_back();
	enPassantHistory.pop_back();
	halfMoveClockHistory.pop_back();

	if (!irreversibleIndices.empty() && irreversibleIndices.back() == positionHistory.size() - 1) irreversibleIndices.pop_back();
	positionHistory.pop_back();

	if (lastMove.player == BLACK) fullMoveClock--;

	if (lastMove.promotionType != EMPTY) {
		removePiece(lastMove.end);
		setPiece(lastMove.start, {PAWN, lastMove.player});
		if (lastMove.flag > 0)
			setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), invertColor(lastMove.player)});
	}
	else {
		movePiece(lastMove.end, lastMove.start);

		if (lastMove.flag > 0 && lastMove.flag < 6) {
			setPiece(lastMove.end, {static_cast<Type>(lastMove.flag), invertColor(lastMove.player)});
		}
		else if (lastMove.flag == ENPASSANT) {
			setPiece(enPassantSquare, {PAWN, invertColor(lastMove.player)});
		}
		else if (lastMove.flag == CASTLEKINGSIDE) {
			movePiece(lastMove.end - 1, lastMove.end + 1);
		}
		else if (lastMove.flag == CASTLEQUEENSIDE) {
			movePiece(lastMove.end + 1, lastMove.end - 2);
		}
	}

	sideToMove = invertColor(sideToMove);
	hashCode ^= hashCodes.blackToMoveCode;
}

bool ChessBoard::isDraw() const {
	// 50 move rule
	if (halfMoveClock >= 100) return true;
	// repetition
	for (int j = positionHistory.size() - 4;
		 j >= 0 && (irreversibleIndices.empty() || irreversibleIndices.back() < j);
		 j -= 2) {
		if (positionHistory[j] == hashCode) {
			return true;
		}
		 }
	// insufficient material
	if (whitePieces.empty() && whitePawns.empty() && blackPieces.empty() && blackPawns.empty()) return true;

	return false;
}

void ChessBoard::movePiece(int_fast8_t start, int_fast8_t end) {
	if (squares[end].type != EMPTY) removePiece(end);

	const Square piece = squares[start];

	squares[end] = squares[start];
	squares[start] = {EMPTY, WHITE};

	switch (piece.type) {
		case PAWN: {
			std::vector<int_fast8_t>* pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
			for (int_fast8_t&i: *pawnList) {
				if (i == start) {
					i = end;
					break;
				}
			}
		}
		break;
		case KING: {
			int_fast8_t* kingPosition = (piece.color == WHITE) ? &whiteKing : &blackKing;
			*kingPosition = end;
		}
		default: {
			std::vector<Piece>* pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
			for (Piece&i: *pieceList) {
				if (i.position == start) {
					i.position = end;
					break;
				}
			}
		}
		break;
	}

	hashCode ^= hashCodes.pieceCode(piece.type, piece.color, start);
	hashCode ^= hashCodes.pieceCode(piece.type, piece.color, end);
}

void ChessBoard::setPiece(int_fast8_t position, const Square&piece) {
	switch (piece.type) {
		case PAWN: {
			std::vector<int_fast8_t>* pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
			pawnList->push_back(position);
		}
		break;
		case KING: {
			int_fast8_t* kingPosition = (piece.color == WHITE) ? &whiteKing : &blackKing;
			*kingPosition = position;
		}
		default: {
			std::vector<Piece>* pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
			pieceList->push_back({piece.type, position});
		}
		break;
	}

	hashCode ^= hashCodes.pieceCode(piece.type, piece.color, position);
	squares[position] = piece;
}

void ChessBoard::removePiece(int_fast8_t position) {
	const Square piece = squares[position];

	if (piece.type == PAWN) {
		std::vector<int_fast8_t>* pawnList = (piece.color == WHITE) ? &whitePawns : &blackPawns;
		for (int i = 0; pawnList->size() > i; i++) {
			if ((*pawnList)[i] == position) {
				pawnList->erase(pawnList->begin() + i);
				break;
			}
		}
	}
	else {
		std::vector<Piece>* pieceList = (piece.color == WHITE) ? &whitePieces : &blackPieces;
		for (int i = 0; pieceList->size() > i; i++) {
			if ((*pieceList)[i].position == position) {
				pieceList->erase(pieceList->begin() + i);
				break;
			}
		}
	}

	hashCode ^= hashCodes.pieceCode(piece.type, piece.color, position);
	squares[position] = {EMPTY, WHITE};
}

void ChessBoard::updateCastlingRights(const Move&move) {
	if (move.player == WHITE) {
		if (castlingRights.whiteKingSide) {
			if (move.start == 60 || move.start == 63) {
				castlingRights.whiteKingSide = false;
			}
		}
		if (castlingRights.whiteQueenSide) {
			if (move.start == 60 || move.start == 56) {
				castlingRights.whiteQueenSide = false;
			}
		}
		if (castlingRights.blackKingSide && move.end == 7) {
			castlingRights.blackKingSide = false;
		}
		else if (castlingRights.blackQueenSide && move.end == 0) {
			castlingRights.blackQueenSide = false;
		}
	}
	else {
		if (castlingRights.blackKingSide) {
			if (move.start == 4 || move.start == 7) {
				castlingRights.blackKingSide = false;
			}
		}
		if (castlingRights.blackQueenSide) {
			if (move.start == 4 || move.start == 0) {
				castlingRights.blackQueenSide = false;
			}
		}
		if (castlingRights.whiteKingSide && move.end == 63) {
			castlingRights.whiteKingSide = false;
		}
		else if (castlingRights.whiteQueenSide && move.end == 56) {
			castlingRights.whiteQueenSide = false;
		}
	}
}

std::string ChessBoard::fen() const {
	std::string fen;
	for (int i = 0; i < 8; ++i) {
		int emptyspaces = 0;
		for (int j = 0; j < 8; ++j) {
			Square square = squares[i * 8 + j];
			if (square.type == EMPTY) {
				emptyspaces++;
				if (j == 7) {
					fen += std::to_string(emptyspaces);
				}
			}
			else {
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

	int_fast8_t fenPassant = enPassantSquare;
	if (fenPassant < 31) fenPassant -= 8;
	else fenPassant += 8;

	fen += (enPassantSquare == -1) ? "-" : Util::positionToString(fenPassant);
	fen += " ";
	fen += std::to_string(halfMoveClock);
	fen += " ";
	fen += std::to_string(fullMoveClock);

	return fen;
}

void ChessBoard::setPosition(const std::string&fen) {
	hashCode = hashCodes.initialCode;
	int_fast8_t position = 0;
	int index = 0;

	whitePieces = std::vector<Piece>();
	blackPieces = std::vector<Piece>();
	whitePawns = std::vector<int_fast8_t>();
	blackPawns = std::vector<int_fast8_t>();
	whiteKing = -1;
	blackKing = -1;
	squares = std::array<Square, 64>();
	enPassantSquare = -1;
	positionHistory = std::vector<uint64_t>();
	irreversibleIndices = std::vector<int>();

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
		}
		else {
			setPiece(position, Util::charToPiece(fen[index]));
			position++;
		}
		index++;
	}

	index++;
	if (fen[index] == 'b') {
		sideToMove = BLACK;
		hashCode ^= hashCodes.blackToMoveCode;
	}
	else sideToMove = WHITE;
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
		hashCode ^= hashCodes.castlingRightCodes[castlingRights.blackKingSide * 8 + castlingRights.blackQueenSide * 4 +
		                                         castlingRights.whiteKingSide * 2 + castlingRights.whiteQueenSide];
	}
	else index++;
	index++;

	if (fen[index] != '-') {
		int_fast8_t fenPassant = Util::stringToPosition(fen.substr(index, 2));
		if (fenPassant < 32) fenPassant += 8;
		else fenPassant -= 8;
		enPassantSquare = fenPassant;
		hashCode ^= hashCodes.enPassantFileCode[enPassantSquare % 8];
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

ChessBoard::ChessBoard() {
	hashCodes.initialize();
}
