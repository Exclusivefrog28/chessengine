#ifndef CHESSENGINE_TRANSPOSITIONTABLE_H
#define CHESSENGINE_TRANSPOSITIONTABLE_H

#include <array>
#include "Move.h"

#define TT_SIZE (1<<12)

class TranspositionTable {
public:
    enum NodeType {
        EXACT = 0,
        LOWERBOUND = 1,
        UPPERBOUND = 2
    };
    struct Entry {
        long int key;
        Moves::Move bestMove;
        int depth;
        int score;
        NodeType nodeType;
    };

    static TranspositionTable &getInstance();

    static bool contains(long int key);

    static Entry getEntry(long int key, int ply);

    static void setEntry(long int key, Entry entry);

private:
    static TranspositionTable *instance;

    std::array<Entry, TT_SIZE> entries;
};


#endif