#include "Evaluator.h"

int Evaluator::evaluate(const ChessBoard &board) {

    int gamePhase = 0;
    int whiteTableMidScore = 0;
    int whiteTableEndScore = 0;
    int blackTableMidScore = 0;
    int blackTableEndScore = 0;

    for (const Piece &piece: board.whitePieces) {
        const int index = piece.type - 1;
        whiteTableMidScore += mg_value[index] + mg_pesto_table[index][piece.position];
        whiteTableEndScore += eg_value[index] + eg_pesto_table[index][piece.position];
        gamePhase += gamephaseInc[index];
    }
    for (const int_fast8_t &pos: board.whitePawns) {
        whiteTableMidScore += mg_value[0] + mg_pesto_table[0][pos];
        whiteTableEndScore += eg_value[0] + eg_pesto_table[0][pos];
    }
    for (const Piece &piece: board.blackPieces) {
        const int index = piece.type - 1;
        const int_fast8_t position = flipPosition(piece.position);
        blackTableMidScore += mg_value[index] + mg_pesto_table[index][position];
        blackTableEndScore += eg_value[index] + eg_pesto_table[index][position];
        gamePhase += gamephaseInc[index];
    }
    for (const int_fast8_t &pos: board.blackPawns) {
        const int_fast8_t position = flipPosition(pos);
        blackTableMidScore += mg_value[0] + mg_pesto_table[0][position];
        blackTableEndScore += eg_value[0] + eg_pesto_table[0][position];
    }

    const int midGameScore = (board.sideToMove == WHITE) ? whiteTableMidScore - blackTableMidScore : blackTableMidScore -
                                                                                                     whiteTableMidScore;
    const int endGameScore = (board.sideToMove == WHITE) ? whiteTableEndScore - blackTableEndScore : blackTableEndScore -
                                                                                                     whiteTableEndScore;

    const int midGamePhase = (gamePhase > 24) ? 24 : gamePhase;
    const int endGamePhase = 24 - midGamePhase;

    return ((midGameScore * midGamePhase + endGameScore * endGamePhase) / 24) + Util::randomOffset();
}

int_fast8_t Evaluator::flipPosition(const int_fast8_t pos) {
    const int_fast8_t file = pos % 8;
    const int_fast8_t rank = pos / 8;

    return (7 - rank) * 8 + file;
}
