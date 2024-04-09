#include <catch2/catch_test_macros.hpp>

#include "CLI.h"
#include "TranspositionTable.h"
#include "catch2/internal/catch_windows_h_proxy.hpp"

#define MATE_SCORE INT32_MAX

auto tt = TranspositionTable();

TEST_CASE("TT - rw", "[TranspositionTests]") {
	ChessBoard board;
	board.setPosition("8/RP1qb1B1/2p2P2/6n1/P7/p3k3/Q3b1BK/6R1 w - - 0 1");
	tt.setEntry(board, Interface::CLI::parseMove("f6e7", board), 1, 1, TranspositionTable::EXACT, 1);
	const auto readEntry = tt.getEntry(board.hashCode, 1);
	CHECK(readEntry.bestMove == Interface::CLI::parseMove("f6e7", board));
	CHECK(readEntry.depth == 1);
	CHECK(readEntry.score == 1);
	CHECK(readEntry.nodeType == TranspositionTable::EXACT);
}

TEST_CASE("TT - overwrite", "[TranspositionTests]") {
	ChessBoard board;
	board.setPosition("6k1/5bPp/8/n3p1PR/7Q/4P1b1/2r3Pp/2NK4 w - - 0 1");

	tt.setEntry(board, Interface::CLI::parseMove("h4b4", board), 2, 1, TranspositionTable::LOWERBOUND, 1);
	auto readEntry = tt.getEntry(board.hashCode, 1);
	CHECK(readEntry.score == 1);

	tt.setEntry(board, Interface::CLI::parseMove("h4b4", board), 1, 2, TranspositionTable::LOWERBOUND, 1);
	readEntry = tt.getEntry(board.hashCode, 1);
	CHECK(readEntry.score == 1);

	tt.setEntry(board, Interface::CLI::parseMove("h4b4", board), 3, 3, TranspositionTable::LOWERBOUND, 1);
	readEntry = tt.getEntry(board.hashCode, 1);
	CHECK(readEntry.score == 3);

	tt.setEntry(board, Interface::CLI::parseMove("h4b4", board), 2, 4, TranspositionTable::EXACT, 1);
	readEntry = tt.getEntry(board.hashCode, 1);
	CHECK(readEntry.score == 4);
}

TEST_CASE("TT - mate recall", "[TranspositionTests]") {
	ChessBoard board;
	board.setPosition("1b6/2N1Pnk1/pp2Pr2/8/2P5/2qp1RN1/7K/5b2 w - - 0 1");
	// mate is 8 ply from root, 3 ply from current node
	tt.setEntry(board, Interface::CLI::parseMove("g3h5", board), 1, MATE_SCORE - 8, TranspositionTable::EXACT, 5);
	// current node is 2 ply from root
	auto readEntry = tt.getEntry(board.hashCode, 2);
	// thus mate is 5 ply from current root
	CHECK(readEntry.score == MATE_SCORE - 5);
}