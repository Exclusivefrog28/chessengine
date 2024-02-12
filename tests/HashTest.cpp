#include <catch2/catch_test_macros.hpp>

#include "CLI.h"

TEST_CASE("HASH - Initial Code", "[HashTests]") {
	ChessBoard board = ChessBoard();
	const HashCodes codes = board.hashCodes;
	board.setPosition("4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1");
	const uint64_t targetCode = codes.initialCode
	                            ^ codes.pieceCode(KING, BLACK, 4)
	                            ^ codes.pieceCode(KING, WHITE, 60)
	                            ^ codes.pieceCode(PAWN, BLACK, 27)
	                            ^ codes.pieceCode(PAWN, WHITE, 36);
	CHECK(board.hashCode == targetCode);
}

TEST_CASE("HASH - Capturing a piece", "[HashTests]") {
	ChessBoard board = ChessBoard();
	const HashCodes codes = board.hashCodes;
	board.setPosition("4k3/8/8/3p4/4P3/8/8/4K3 w - - 0 1");
	const uint64_t originalCode = board.hashCode;
	board.makeMove(Interface::CLI::parseMove("e4d5", board));
	const uint64_t targetCode = originalCode
	                            ^ codes.pieceCode(PAWN, WHITE, 36)
	                            ^ codes.pieceCode(PAWN, BLACK, 27)
	                            ^ codes.pieceCode(PAWN, WHITE, 27)
	                            ^ codes.blackToMoveCode;
	CHECK(board.hashCode == targetCode);
}

TEST_CASE("HASH - Losing one castling right", "[HashTests]") {
	ChessBoard board = ChessBoard();
	const HashCodes codes = board.hashCodes;
	board.setPosition("4k3/8/8/3p4/4P3/8/8/R3K2R w KQ - 0 1");
	const uint64_t originalCode = board.hashCode;
	board.makeMove(Interface::CLI::parseMove("h1h2", board));
	const uint64_t targetCode = originalCode
	                            ^ codes.pieceCode(ROOK, WHITE, 63)
	                            ^ codes.pieceCode(ROOK, WHITE, 55)
	                            ^ codes.blackToMoveCode
	                            ^ codes.castlingRightCodes[3]
	                            ^ codes.castlingRightCodes[1];
	CHECK(board.hashCode == targetCode);
}

TEST_CASE("HASH - Losing two castling rights", "[HashTests]") {
	ChessBoard board = ChessBoard();
	const HashCodes codes = board.hashCodes;
	board.setPosition("4k3/8/8/3p4/4P3/8/8/R3K2R w KQ - 0 1");
	const uint64_t originalCode = board.hashCode;
	board.makeMove(Interface::CLI::parseMove("e1e2", board));
	const uint64_t targetCode = originalCode
	                            ^ codes.pieceCode(KING, WHITE, 60)
	                            ^ codes.pieceCode(KING, WHITE, 52)
	                            ^ codes.blackToMoveCode
	                            ^ codes.castlingRightCodes[3]
	                            ^ codes.castlingRightCodes[0];
	CHECK(board.hashCode == targetCode);
}

TEST_CASE("HASH - En passant", "[HashTests]") {
	ChessBoard board = ChessBoard();
	const HashCodes codes = board.hashCodes;
	board.setPosition("4k3/3p4/8/8/8/8/4P3/4K3 w - - 0 1");
	uint64_t originalCode = board.hashCode;
	board.makeMove(Interface::CLI::parseMove("e2e4", board));
	uint64_t targetCode = originalCode
	                      ^ codes.pieceCode(PAWN, WHITE, 52)
	                      ^ codes.pieceCode(PAWN, WHITE, 36)
	                      ^ codes.blackToMoveCode
	                      ^ codes.enPassantFileCode[4];
	CHECK(board.hashCode == targetCode);
	originalCode = board.hashCode;
	board.makeMove(Interface::CLI::parseMove("d7d6", board));
	targetCode = originalCode
	             ^ codes.pieceCode(PAWN, BLACK, 11)
	             ^ codes.pieceCode(PAWN, BLACK, 19)
	             ^ codes.blackToMoveCode
	             ^ codes.enPassantFileCode[4];
	CHECK(board.hashCode == targetCode);
}
