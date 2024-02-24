#ifndef CHESSENGINE_UTIL_H
#define CHESSENGINE_UTIL_H

#include <array>
#include <algorithm>
#include "Piece.h"

namespace Util {
	using namespace Pieces;

	static std::string pieceToString(const Type type, const Color color) {
		std::string piece;
		switch (type) {
			case PAWN:
				piece = 'p';
				break;
			case KNIGHT:
				piece = 'n';
				break;
			case BISHOP:
				piece = 'b';
				break;
			case ROOK:
				piece = 'r';
				break;
			case QUEEN:
				piece = 'q';
				break;
			case KING:
				piece = 'k';
				break;
			case EMPTY:
				break;
		}
		if (color == WHITE) std::ranges::transform(piece, piece.begin(), ::toupper);
		return piece;
	}

	static Square charToPiece(const char c) {
		Square p;
		const char C = c;
		switch (toupper(C)) {
			case 'P':
				p.type = PAWN;
				break;
			case 'N':
				p.type = KNIGHT;
				break;
			case 'B':
				p.type = BISHOP;
				break;
			case 'R':
				p.type = ROOK;
				break;
			case 'Q':
				p.type = QUEEN;
				break;
			case 'K':
				p.type = KING;
				break;
			default:
				p.type = EMPTY;
				break;
		}
		if (c >= 'a' && c <= 'z') p.color = BLACK;
		else p.color = WHITE;

		return p;
	}

	const std::array<std::string, 64> positionToStringMap = {
		"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
		"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
		"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
		"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
		"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
		"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
		"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
		"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
	};

	static std::string positionToString(const int_fast8_t position) {
		return positionToStringMap[position];
	}

	static int_fast8_t stringToPosition(const std::string&position) {
		const int_fast8_t file = position[0] - 'a';
		const int_fast8_t rank = 8 - (position[1] - '0');
		return rank * 8 + file;
	}
}
#endif //CHESSENGINE_UTIL_H
