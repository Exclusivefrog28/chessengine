#ifndef CHESSENGINE_TRANSPOSITIONTABLE_H
#define CHESSENGINE_TRANSPOSITIONTABLE_H

#include <array>
#include "Move.h"
#include <cstdint>

#include "ChessBoard.h"

// 256 megabytes
#define TT_SIZE (1<<23)

class TranspositionTable {
public:
    enum NodeType {
        EMPTY = 0,
        EXACT = 1,
        LOWERBOUND = 2,
        UPPERBOUND = 3,
    };

    struct Entry {
        uint64_t key;
        Move bestMove{};
        uint32_t data;

        uint8_t depth() const {
            return (data & 0xFF0000) >> 16;
        }

        int16_t score() const {
            return data & 0xFFFF;
        }

        void setScore(int16_t score) {
            data = (data & 0xFFF0000) | score;
        }

        NodeType nodeType() const {
            return (NodeType) ((data & 0xF000000) >> 24);
        }

        void setNodeType(NodeType nodeType) {
            data = (data & 0x0FFFFFF) | (((uint8_t) nodeType) << 24);
        }

        Entry(uint64_t key, Move bestMove, int depth, int score, NodeType nodeType) {
            this->key = key;
            this->bestMove = bestMove;
            this->data = 0 | (0xFFFF & ((int16_t) score)) | (((uint8_t) depth) << 16) | (((uint8_t) nodeType) << 24);
        }

        Entry() = default;
    };

    TranspositionTable();

    bool contains(uint64_t key);

    Entry getEntry(uint64_t key, int ply);

    void setEntry(const ChessBoard &board, Move bestMove, int depth, int score, NodeType nodeType, int ply);

    int reads;
    int writes;
    int collisions;

    void resetCounters();

    int occupancy() const;

    void clear();

    std::array<Entry, TT_SIZE> entries;

private:
    void write(int index, const Entry &entry);


};

#endif