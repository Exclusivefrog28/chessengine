#include <catch2/catch_test_macros.hpp>
#include "../src/CLI.h"

TEST_CASE("Repetition by White", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P1Q1PP1/2R1R1K1 b - - 6 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(board.isRepetition());
}

TEST_CASE("Repetition by Black", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(board.isRepetition());
}

TEST_CASE("No repetition by White", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P1Q1PP1/2R1R1K1 b - - 6 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("a4b6", board));
	CHECK(!board.isRepetition());
}

TEST_CASE("No repetition by Black", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("c6a5", board));
	CHECK(!board.isRepetition());
}

TEST_CASE("Repetition after irreversible move", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("h4h5", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(board.isRepetition());
}

TEST_CASE("Irreversible Moves", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	CHECK(board.irreversibleIndices.empty());
	CHECK(board.positionHistory.empty());
	// PAWN move
	board.makeMove(Interface::CLI::parseMove("b2b4", board));
	CHECK(board.positionHistory.size() == 1);
	CHECK(board.irreversibleIndices.size() == 1);
	CHECK(board.irreversibleIndices[0] == 0);
	// CAPTURE
	board.makeMove(Interface::CLI::parseMove("c6b4", board));
	CHECK(board.positionHistory.size() == 2);
	CHECK(board.irreversibleIndices.size() == 2);
	CHECK(board.irreversibleIndices[1] == 1);
	// QUIET MOVE
	board.makeMove(Interface::CLI::parseMove("a4c3", board));
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

TEST_CASE("Repetition after changing position", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 9 25");
	CHECK(board.isRepetition());
}

TEST_CASE("No repetition after changing position", "[RepetitionTests]") {
	ChessBoard board = ChessBoard();
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.isRepetition());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.isRepetition());
	board.setPosition("2r2rk1/1q1b1pp1/3pp2p/n2p4/N2PnN1P/4PB2/1P3PP1/2RQR1K1 w - - 9 25");
	CHECK(!board.isRepetition());
}