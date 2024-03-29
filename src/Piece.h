#ifndef CHESSENGINE_PIECE_H
#define CHESSENGINE_PIECE_H

namespace Pieces {
	enum Type {
		EMPTY = 0,
		PAWN = 1,
		KNIGHT = 2,
		BISHOP = 3,
		ROOK = 4,
		QUEEN = 5,
		KING = 6
	};

	enum Color {
		WHITE = 0,
		BLACK = 1
	};

	struct Piece {
		Type type;
		int_fast8_t position;
	};

	struct Square {
		Type type;
		Color color;

		bool operator==(const Square&other) const {
			return type == other.type && color == other.color;
		}
	};

	static Color invertColor(Color color) {
		if (color == WHITE) return BLACK;
		else return WHITE;
	}
}

#endif //CHESSENGINE_PIECE_H
