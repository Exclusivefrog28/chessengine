#include "TranspositionTable.h"
#include <cmath>

#include "MoveGenerator.h"

#define MATE_SCORE 32768 // 1 << 15
#define MIN_MATE_SCORE 16384 // 1 << 14

TranspositionTable::Entry TranspositionTable::getEntry(const uint64_t key, const int ply) {
	const int index = key % TT_SIZE;
	Entry entry = entries[index];

	if (abs(entry.score()) >= MIN_MATE_SCORE) {
		const int sign = entry.score() > 0 ? 1 : -1;
		entry.setScore(sign * (abs(entry.score()) - ply));
	}
	reads++;

	return entry;
}

bool TranspositionTable::contains(const uint64_t key) {
	const int index = key % TT_SIZE;
	const bool exists = entries[index].nodeType() != EMPTY;
	const bool sameKey = (entries[index].key == key);
	if (exists && !sameKey) collisions++;

	return exists && sameKey;
}

void TranspositionTable::setEntry(const ChessBoard&board, const Move bestMove, const int depth, int score, const NodeType nodeType, const int ply) {
	const int index = board.hashCode % TT_SIZE;

	if (abs(score) >= MIN_MATE_SCORE) {
		const int sign = score > 0 ? 1 : -1;
		score = sign * (abs(score) + ply);
	}

	const NodeType savedType = entries[index].nodeType();

	const Entry entry = Entry(board.hashCode, bestMove, depth, score, nodeType);

	//REPLACEMENT SCHEME
	// 1. Prefer EXACT nodes to bounds
	// 2. Prefer deeper nodes to shallower

	if (savedType != EMPTY) {
		if ((savedType != EXACT && nodeType != EXACT) || (savedType == EXACT && nodeType == EXACT)) {
			if (entries[index].depth() <= depth) write(index, entry);
		}
		else if (savedType != EXACT) write(index, entry);
	}
	else write(index, entry);
}

void TranspositionTable::write(const int index, const Entry&entry) {
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
		if (entries[i].nodeType() != EMPTY) occupied++;
	}
	return occupied;
}

void TranspositionTable::clear() {
	resetCounters();
	for (Entry&entry: entries) {
		entry.setNodeType(EMPTY);
	}
}

TranspositionTable::TranspositionTable()= default;
