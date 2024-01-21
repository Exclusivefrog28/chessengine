#include <catch2/catch_test_macros.hpp>
#include "../src/CLI.h"

TEST_CASE("Repetition by White", "[RepetitionTests]") {
	ChessBoard board;
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P1Q1PP1/2R1R1K1 b - - 6 23");
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(board.hasRepetitions());
}

TEST_CASE("Repetition by Black", "[RepetitionTests]") {
	ChessBoard board;
	board.setPosition("2r2rk1/1q1b1pp1/2nppn1p/3p4/N2P1N1P/4PB2/1P3PP1/2RQR1K1 w - - 5 23");
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("d1d2", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("f6e4", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("d2d1", board));
	CHECK(!board.hasRepetitions());
	board.makeMove(Interface::CLI::parseMove("e4f6", board));
	CHECK(board.hasRepetitions());
}
