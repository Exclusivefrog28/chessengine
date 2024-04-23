#include "TranspositionTable.h"
#include <cmath>
#include <fstream>

#include "MoveParser.h"

#define MATE_SCORE INT32_MAX
#define MIN_MATE_SCORE (INT32_MAX - 1024)

TranspositionTable::Entry TranspositionTable::getEntry(const uint64_t key, const int ply) {
    const int index = key % TT_SIZE;
    Entry entry = entries[index];

    if (isMateScore(entry.score)) {
        const int sign = entry.score > 0 ? 1 : -1;
        entry.score = sign * (abs(entry.score) - ply);
    }
    reads++;

    return entry;
}

bool TranspositionTable::contains(const uint64_t key) {
    const int index = key % TT_SIZE;
    const bool exists = entries[index].nodeType != EMPTY;
    const bool sameKey = (entries[index].key == key);
    if (exists && !sameKey) collisions++;

    return exists && sameKey;
}

void TranspositionTable::setEntry(const ChessBoard &board, const Move bestMove, const int depth, int score,
                                  const NodeType nodeType, const int ply) {
    const int index = board.hashCode % TT_SIZE;

    if (isMateScore(score)) {
        const int sign = score > 0 ? 1 : -1;
        score = sign * (abs(score) + ply);
    }

    const NodeType savedType = entries[index].nodeType;

    const Entry entry = {board.hashCode, bestMove, score, static_cast<int8_t>(depth), nodeType};

    //REPLACEMENT SCHEME
    // 1. Prefer EXACT nodes to bounds
    // 2. Prefer deeper nodes to shallower

    if (savedType != EMPTY && savedType != BOOK) {
        if ((savedType != EXACT && nodeType != EXACT) || (savedType == EXACT && nodeType == EXACT)) {
            if (entries[index].depth <= depth) write(index, entry);
        } else if (savedType != EXACT) write(index, entry);
    } else write(index, entry);
}

void TranspositionTable::write(const int index, const Entry &entry) {
    entries[index] = entry;
    writes++;
}

void TranspositionTable::resetCounters() {
    reads = 0;
    writes = 0;
    collisions = 0;
}

int TranspositionTable::occupancy() const {
    int occupied = 0;
    for (int i = 0; i < TT_SIZE; i++) {
        if (entries[i].nodeType != EMPTY) occupied++;
    }
    return occupied;
}

void TranspositionTable::clear() {
    resetCounters();
    for (Entry &entry: entries) {
        entry.nodeType = EMPTY;
    }
}

void TranspositionTable::loadOpenings(ChessBoard &board) {
    std::ifstream file("opening_book.txt");

    if (!file.is_open()) {
        std::cerr << "Failed to open opening book" << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line);
    entries[board.hashCode % TT_SIZE] = {board.hashCode, parseMove(line,board), 0, 1, BOOK};

    while (!file.eof()) {
        std::getline(file, line);
        if (line[0] == 'm') {
            std::getline(file, line);
            board.makeMove(parseMove(line, board));

            std::getline(file, line);
            entries[board.hashCode % TT_SIZE] = {board.hashCode, parseMove(line,board), 0, 1, BOOK};
        } else if (line[0] == 'u'){
            board.unMakeMove();
        }
    }

    file.close();
}

bool TranspositionTable::isMateScore(int32_t score) {
    return abs(score) >= MIN_MATE_SCORE;
}
