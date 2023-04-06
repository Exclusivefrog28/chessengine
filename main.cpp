#include <iostream>
#include "ChessBoard.h"
#include "Piece.h"
#include "Move.h"

int main() {
    ChessBoard board;

    board.setStartingPosition();

    std::cout << board << std::endl;
    board.makeMove({7*8+1,5*8+2, static_cast<MoveFlag>(0), WHITE});
    std::cout << board << std::endl;
    board.unMakeMove();
    std::cout << board << std::endl;

    return 0;
}
