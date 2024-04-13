#include <catch2/catch_test_macros.hpp>

#include "ChessBoard.h"
#include "MoveParser.h"

TEST_CASE("Repetitions - by White", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P1Q1PP1/2R1R1K1 b - - 6 23");
	CHECK(!board.isDraw());
	board.makeMove(parseMove("f6e4", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d2d1", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("e4f6", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d1d2", board));
	CHECK(board.isDraw());
}

TEST_CASE("Repetitions - by Black", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d1d2", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("f6e4", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d2d1", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("e4f6", board));
	CHECK(board.isDraw());
}

TEST_CASE("Repetitions - No repetition by White", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P1Q1PP1/2R1R1K1 b - - 6 23");
	CHECK(!board.isDraw());
	board.makeMove(parseMove("f6e4", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d2d1", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("e4f6", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("a4b6", board));
	CHECK(!board.isDraw());
}

TEST_CASE("Repetitions - No repetition by Black", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d1d2", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("f6e4", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d2d1", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("c6a5", board));
	CHECK(!board.isDraw());
}

TEST_CASE("Repetitions - after irreversible move", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isDraw());
	board.makeMove(parseMove("h4h5", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("f6e4", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d1d2", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("e4f6", board));
	CHECK(!board.isDraw());
	board.makeMove(parseMove("d2d1", board));
	CHECK(board.isDraw());
}

TEST_CASE("Repetitions - Irreversible Moves", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isDraw());
	CHECK(board.irreversibleIndices.empty());
	CHECK(board.positionHistory.empty());
	// PAWN move
	board.makeMove(parseMove("b2b4", board));
	CHECK(board.positionHistory.size() == 1);
	CHECK(board.irreversibleIndices.size() == 1);
	CHECK(board.irreversibleIndices[0] == 0);
	// CAPTURE
	board.makeMove(parseMove("c6b4", board));
	CHECK(board.positionHistory.size() == 2);
	CHECK(board.irreversibleIndices.size() == 2);
	CHECK(board.irreversibleIndices[1] == 1);
	// QUIET MOVE
	board.makeMove(parseMove("a4c3", board));
	CHECK(board.positionHistory.size() == 3);
	CHECK(board.irreversibleIndices.size() == 2);
	board.unMakeMove();
	CHECK(board.positionHistory.size() == 2);
	CHECK(board.irreversibleIndices.size() == 2);
	board.unMakeMove();
	CHECK(board.positionHistory.size() == 1);
	CHECK(board.irreversibleIndices.size() == 1);
	board.unMakeMove();
	CHECK(board.positionHistory.empty());
	CHECK(board.irreversibleIndices.empty());
	CHECK(board.fen() == "2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
}