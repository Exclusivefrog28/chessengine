#ifndef CHESSENGINE_HASHCODES_H
#define CHESSENGINE_HASHCODES_H

#include <array>
#include <random>
#include <cstdint>

class HashCodes {
public:
    uint64_t initialCode;
    std::array<uint64_t, 12 * 64> pieceCodes;
    uint64_t blackToMoveCode;
    std::array<uint64_t, 16> castlingRightCodes;
    std::array<uint64_t, 8> enPassantFileCode;

    void initialize() {
        std::random_device dev;
        std::mt19937_64 rng(dev());
        std::uniform_int_distribution dist(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max());

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

    uint64_t pieceCode(const Pieces::Type type, const Pieces::Color color, const short position) const {
        return pieceCodes[((type - 1) * 64) + (color * 6*64) + position];
    }
};

#endif //CHESSENGINE_HASHCODES_H
