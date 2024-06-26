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
        FREE = 0,
        EXACT = 1,
        LOWERBOUND = 2,
        UPPERBOUND = 3,
        BOOK = 4
    };
    struct Entry {
        uint64_t key: 64;
        Move bestMove;
        int32_t score: 32;
        int8_t depth: 8;
        NodeType nodeType: 3;
    };

    bool contains(uint64_t key);

    Entry getEntry(uint64_t key, int ply);

    void setEntry(const ChessBoard &board, Move bestMove, int depth, int score, NodeType nodeType, int ply);

    static bool isMateScore(int32_t score) ;

    int reads = 0;
    int writes = 0;
    int collisions = 0;
    int entryCount = 0;

    void resetCounters();

    void clear();

    void loadOpenings(ChessBoard &board);

    std::array<Entry, TT_SIZE> entries{};

private:
    void write(int index, const Entry &entry);


};

#endif