#ifndef CHESSENGINE_HASHCODES_H
#define CHESSENGINE_HASHCODES_H

#include <array>
#include <random>

class HashCodes {
public:
    unsigned long int initialCode;
    std::array<unsigned long int, 12 * 64> pieceCodes;
    unsigned long int blackToMoveCode;
    std::array<unsigned long int, 16> castlingRightCodes;
    std::array<unsigned long int, 8> enPassantFileCode;

    void initialize() {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<unsigned long int> dist(std::numeric_limits<unsigned long int>::min(), std::numeric_limits<unsigned long int>::max());

        initialCode = dist(rng);

        for (int i = 0; i < 12 * 64; ++i) {
            pieceCodes[i] = dist(rng);
        }
        blackToMoveCode = dist(rng);
        for (int i = 0; i < 16; ++i) {
            castlingRightCodes[i] = dist(rng);
        }
        for (int i = 0; i < 8; ++i) {
            enPassantFileCode[i] = dist(rng);
        }
    }

    unsigned long int pieceCode(Pieces::Type type, Pieces::Color color, short position) {
        return pieceCodes[((type - 1) * 64) + (color * 6*64) + position];
    }
};

#endif //CHESSENGINE_HASHCODES_H
