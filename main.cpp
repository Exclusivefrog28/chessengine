#include <iostream>
#include "ChessBoard.h"

int main() {

    ChessBoard board;

    board.setStartingPosition();

    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            int piece = board.squares[i*8+j].type;
            std::string code;
            switch (piece) {
                case 1:
                    code = "p";
                    break;
                case 2:
                    code = "N";
                    break;
                case 3:
                    code = "B";
                    break;
                case 4:
                    code = "R";
                    break;
                case 5:
                    code = "Q";
                    break;
                case 6:
                    code = "K";
                    break;
                default:
                    code = "-";
                    break;
            }
            std::cout << code << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
