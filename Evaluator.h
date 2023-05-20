#ifndef CHESSENGINE_EVALUATOR_H
#define CHESSENGINE_EVALUATOR_H

#include "ChessBoard.h"
#include "EvaluationValues.h"

using namespace EvaluationValues;

class Evaluator {

public:
    static int evaluate(const ChessBoard &board);


private:
    static short flipPosition(short pos);
};
#endif //CHESSENGINE_EVALUATOR_H
