#ifndef CHESSENGINE_CHESSBOARD_H
#define CHESSENGINE_CHESSBOARD_H


#include <vector>
#include <cstdint>
#include "Move.h"
#include "Piece.h"
#include "HashCodes.h"

using namespace Pieces;
using namespace Moves;

class ChessBoard {

public:

    struct CastlingRights {
        bool blackKingSide;
        bool blackQueenSide;
        bool whiteKingSide;
        bool whiteQueenSide;

        bool operator ==(const CastlingRights &other) const{
            return blackKingSide == other.blackKingSide &&
                   blackQueenSide == other.blackQueenSide &&
                   whiteKingSide == other.whiteKingSide &&
                   whiteQueenSide == other.whiteQueenSide;
        }
    };

    std::array<Square, 64> squares{};

    std::vector<Piece> whitePieces;
    std::vector<Piece> blackPieces;

    std::vector<int_fast8_t> whitePawns;
    std::vector<int_fast8_t> blackPawns;

    int_fast8_t whiteKing{};
    int_fast8_t blackKing{};

    Color sideToMove = WHITE;
    CastlingRights castlingRights{};
    int_fast8_t enPassantSquare = -1;

    int halfMoveClock = 0;
    int fullMoveClock = 1;

    std::vector<Move> moveHistory;
    std::vector<CastlingRights> castlingRightHistory;
    std::vector<int_fast8_t> enPassantHistory;
    std::vector<int> halfMoveClockHistory;

    std::vector<uint64_t> positionHistory;
    std::vector<int> irreversibleIndices; //indices of positionHistory from where irreversible moves were made

    uint64_t hashCode{};
    HashCodes hashCodes{};

    ChessBoard();

    void setStartingPosition();

    void setPosition(const std::string&fen);

    friend std::ostream &operator<<(std::ostream &os, const ChessBoard &board);

    std::string fen() const;

    void makeMove(const Move &move);

    void unMakeMove();

    bool isRepetition() const;


private:
    void movePiece(int_fast8_t start, int_fast8_t end);

    void setPiece(int_fast8_t position,const Square &piece);

    void removePiece(int_fast8_t position);

    void updateCastlingRights(const Move&move);

};


#endif //CHESSENGINE_CHESSBOARD_H
