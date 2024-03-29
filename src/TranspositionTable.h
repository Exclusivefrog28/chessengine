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
        Move bestMove;
        int depth;
        int score;
        NodeType nodeType;
    };

    bool contains(uint64_t key);

    Entry getEntry(uint64_t key, int ply);

    void setEntry(const ChessBoard&board, Move bestMove, int depth, int score, NodeType nodeType, int ply);

    int reads;
    int writes;
    int collisions;
    void resetCounters();
    int occupancy() const;
    void clear();

    std::array<Entry, TT_SIZE> entries;

private:
    void write(int index, const Entry&entry);


};

#endif