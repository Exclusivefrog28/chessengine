#include <catch2/catch_test_macros.hpp>

#include "Util.h"

TEST_CASE("Util - pieceToString", "[Util]") {
	CHECK(Util::pieceToString(Pieces::PAWN, Pieces::BLACK) == "p");
	CHECK(Util::pieceToString(Pieces::KNIGHT, Pieces::BLACK) == "n");
	CHECK(Util::pieceToString(Pieces::BISHOP, Pieces::BLACK) == "b");
	CHECK(Util::pieceToString(Pieces::ROOK, Pieces::BLACK) == "r");
	CHECK(Util::pieceToString(Pieces::QUEEN, Pieces::BLACK) == "q");
	CHECK(Util::pieceToString(Pieces::KING, Pieces::BLACK) == "k");

	CHECK(Util::pieceToString(Pieces::PAWN, Pieces::WHITE) == "P");
	CHECK(Util::pieceToString(Pieces::KNIGHT, Pieces::WHITE) == "N");
	CHECK(Util::pieceToString(Pieces::BISHOP, Pieces::WHITE) == "B");
	CHECK(Util::pieceToString(Pieces::ROOK, Pieces::WHITE) == "R");
	CHECK(Util::pieceToString(Pieces::QUEEN, Pieces::WHITE) == "Q");
	CHECK(Util::pieceToString(Pieces::KING, Pieces::WHITE) == "K");
}

TEST_CASE("Util - charToPiece", "[Util]") {
	CHECK(Pieces::Square(Pieces::PAWN, Pieces::BLACK) == Util::charToPiece('p'));
	CHECK(Pieces::Square(Pieces::KNIGHT, Pieces::BLACK) == Util::charToPiece('n'));
	CHECK(Pieces::Square(Pieces::BISHOP, Pieces::BLACK) == Util::charToPiece('b'));
	CHECK(Pieces::Square(Pieces::ROOK, Pieces::BLACK) == Util::charToPiece('r'));
	CHECK(Pieces::Square(Pieces::QUEEN, Pieces::BLACK) == Util::charToPiece('q'));
	CHECK(Pieces::Square(Pieces::KING, Pieces::BLACK) == Util::charToPiece('k'));

	CHECK(Pieces::Square(Pieces::PAWN, Pieces::WHITE) == Util::charToPiece('P'));
	CHECK(Pieces::Square(Pieces::KNIGHT, Pieces::WHITE) == Util::charToPiece('N'));
	CHECK(Pieces::Square(Pieces::BISHOP, Pieces::WHITE) == Util::charToPiece('B'));
	CHECK(Pieces::Square(Pieces::ROOK, Pieces::WHITE) == Util::charToPiece('R'));
	CHECK(Pieces::Square(Pieces::QUEEN, Pieces::WHITE) == Util::charToPiece('Q'));
	CHECK(Pieces::Square(Pieces::KING, Pieces::WHITE) == Util::charToPiece('K'));
}

TEST_CASE("Util - positionToString", "[Util]") {
	CHECK(Util::positionToString(0) == "a8");
	CHECK(Util::positionToString(7) == "h8");
	CHECK(Util::positionToString(8) == "a7");
	CHECK(Util::positionToString(35) == "d4");
	CHECK(Util::positionToString(36) == "e4");
	CHECK(Util::positionToString(55) == "h2");
	CHECK(Util::positionToString(56) == "a1");
	CHECK(Util::positionToString(63) == "h1");
}

TEST_CASE("Util - stringToPosition", "[Util]") {
	CHECK(0 == Util::stringToPosition("a8"));
	CHECK(7 == Util::stringToPosition("h8"));
	CHECK(8 == Util::stringToPosition("a7"));
	CHECK(35 == Util::stringToPosition("d4"));
	CHECK(36 == Util::stringToPosition("e4"));
	CHECK(55 == Util::stringToPosition("h2"));
	CHECK(56 == Util::stringToPosition("a1"));
	CHECK(63 == Util::stringToPosition("h1"));
}