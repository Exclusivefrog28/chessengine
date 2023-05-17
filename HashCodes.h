#ifndef CHESSENGINE_HASHCODES_H
#define CHESSENGINE_HASHCODES_H

#include <array>
#include <random>

class HashCodes {
public:
    long int initialCode;
    std::array<long int, 12 * 64> pieceCodes;
    long int blackToMoveCode;
    std::array<long int, 16> castlingRightCodes;
    std::array<long int, 8> enPassantFileCode;

    void initialize() {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution<long int> dist(std::numeric_limits<long int>::min(), std::numeric_limits<long int>::max());

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

    long int pieceCode(Pieces::Type type, Pieces::Color color, short position) {
        return pieceCodes[((type - 1) * 64) + (color * 6*64) + position];
    }
};

#endif //CHESSENGINE_HASHCODES_H
