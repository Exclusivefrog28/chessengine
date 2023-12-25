#include <iostream>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"
#include "Util.h"
#include "MoveGenerator.h"
#include "Evaluator.h"
#include "Search.h"
#include <cstring>

#include "CLI.h"

#ifdef wasm
#include <emscripten/em_macros.h>
#endif

ChessBoard board;

extern "C" {
#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
void init() {
	board.hashCodes.initialize();
	board.setStartingPosition();
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* move(const int start, const int end, int flag, int promotionType, int player) {
	board.makeMove({
		static_cast<short>(start), static_cast<short>(end), static_cast<Pieces::Type>(promotionType),
		static_cast<MoveFlag>(flag), static_cast<Pieces::Color>(player)
	});

	const std::string fen = board.fen();
	const int length = fen.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, fen.c_str());
	return chararray;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* unmove() {
	board.unMakeMove();

	const std::string fen = board.fen();
	const int length = fen.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, fen.c_str());
	return chararray;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* listPieces() {
	std::string string = "White pieces: ";
	for (Piece&piece: board.whitePieces) {
		string += "[";
		string += Util::pieceToString(piece.type, Pieces::WHITE);
		string += ", ";
		string += std::to_string(piece.position);
		string += "] ";
	}
	string += "\nBlack pieces:";
	for (Piece&piece: board.blackPieces) {
		string += "[";
		string += Util::pieceToString(piece.type, Pieces::BLACK);
		string += ", ";
		string += std::to_string(piece.position);
		string += "] ";
	}
	string += "\n";

	const int length = string.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, string.c_str());
	return chararray;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* getMoves() {
	std::string string;
	std::vector<Move> moves = MoveGenerator::pseudoLegalMoves(board);

	bool empty = true;

	string += "{";
	string += R"("moves" : [)";
	for (const Move move: moves) {
		if (MoveGenerator::isLegalMove(board, move)) {
			empty = false;
			string += "{";
			string += R"("start":")";
			string += Util::positionToString(move.start);
			string += R"(","end":")";
			string += Util::positionToString(move.end);
			string += R"(","promotionType":")";
			string += std::to_string(move.promotionType);
			string += R"(","flag":")";
			string += std::to_string(move.flag);
			string += R"(","player":")";
			string += std::to_string(move.player);
			string += "\"},";
		}
	}
	if (!empty) {
		string.pop_back();
	}
	string += R"(],"state":)";

	if (empty) {
		if (MoveGenerator::inCheck(board, board.sideToMove)) string += R"("checkmate")";
		else string += R"("stalemate")";
	}
	else string += R"("normal")";

	string += "}";

	const int length = string.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, string.c_str());
	return chararray;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* getAttacks() {
	std::string attackedSquares;
	for (short i = 0; i < 64; ++i) {
		if (MoveGenerator::isSquareAttacked(board, i, board.sideToMove)) {
			attackedSquares += Util::positionToString(i);
			attackedSquares += " ";
		}
	}
	if (!attackedSquares.empty()) attackedSquares.pop_back();

	const int length = attackedSquares.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, attackedSquares.c_str());
	return chararray;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
int eval() {
	int score;
	if (Search::tt.contains(board.hashCode)) {
		TranspositionTable::Entry entry = Search::tt.getEntry(board.hashCode, 0);
		score = entry.score;
	}
	else {
		score = Evaluator::evaluate(board);
	}
	return score;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
char* getBestMove(int seconds) {
	Move bestMove = Search::search(board, seconds);
	std::string bestMoveJSON = R"({"start":")";
	bestMoveJSON += Util::positionToString(bestMove.start);
	bestMoveJSON += R"(","end":")";
	bestMoveJSON += Util::positionToString(bestMove.end);
	bestMoveJSON += R"(","promotionType":")";
	bestMoveJSON += std::to_string(bestMove.promotionType);
	bestMoveJSON += R"(","flag":")";
	bestMoveJSON += std::to_string(bestMove.flag);
	bestMoveJSON += R"(","player":")";
	bestMoveJSON += std::to_string(bestMove.player);
	bestMoveJSON += "\"}";
	const int length = bestMoveJSON.length();
	char* chararray = new char[length + 1];
	strcpy(chararray, bestMoveJSON.c_str());
	return chararray;
}


#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
int setFen(char* fen) {
	std::string fenString(fen);
	board.setPosition(fenString);
	return board.sideToMove;
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
int runPerft(int depth, const char* fen) {
	ChessBoard perftBoard;
	perftBoard.setPosition(fen);

	return MoveGenerator::perft(depth, perftBoard);
}

#ifdef wasm
EMSCRIPTEN_KEEPALIVE
#endif
int main() {
#ifndef wasm
	Interface::CLI interface;
	interface.start();
#endif
	return 0;
}
}
