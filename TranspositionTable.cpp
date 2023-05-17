#include <cstdio>
#include "TranspositionTable.h"

#define MATE_SCORE 65536

TranspositionTable *TranspositionTable::instance = nullptr;

TranspositionTable &TranspositionTable::getInstance() {
    if (instance == nullptr) {
        instance = new TranspositionTable();
    }
    return *instance;
}

TranspositionTable::Entry TranspositionTable::getEntry(long key, int ply) {
    TranspositionTable tt = TranspositionTable::getInstance();
    int index = key % TT_SIZE;
    Entry entry = tt.entries[index];

    if (abs(entry.score) == MATE_SCORE) {
        int sign = entry.score > 0 ? 1 : -1;
        entry.score = sign * (MATE_SCORE - ply);
    }

    return entry;
}

bool TranspositionTable::contains(long key) {
    TranspositionTable tt = TranspositionTable::getInstance();
    int index = key % TT_SIZE;
    return (tt.entries[index].key == key);
}

void TranspositionTable::setEntry(long key, TranspositionTable::Entry entry) {
    TranspositionTable tt = TranspositionTable::getInstance();
    int index = key % TT_SIZE;

    if (abs(entry.score) >= MATE_SCORE - entry.depth) {
        int sign = entry.score > 0 ? 1 : -1;
        entry.score = sign * MATE_SCORE;
    }

    tt.entries[index] = entry;
}
