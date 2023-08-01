#ifndef CHESSENGINE_TRANSPOSITIONTABLE_H
#define CHESSENGINE_TRANSPOSITIONTABLE_H

#include <array>
#include "Move.h"

// 64 megabytes
#define TT_SIZE (1<<21)

class TranspositionTable {
public:
    enum NodeType {
        EMPTY = 0,
        EXACT = 1,
        LOWERBOUND = 2,
        UPPERBOUND = 3,
    };
    struct Entry {
        unsigned long int key;
        Moves::Move bestMove;
        int depth;
        int score;
        NodeType nodeType;
    };

    bool contains(unsigned long int key);

    Entry getEntry(unsigned long int key, int ply);

    void setEntry(unsigned long int key, Entry entry, int ply);

    int reads;
    int writes;
    int collisions;
    void resetCounters();

    std::array<Entry, TT_SIZE> entries;

private:
    void write(int index, Entry entry);


};

#endif