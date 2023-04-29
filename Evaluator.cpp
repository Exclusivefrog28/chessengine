#include "Evaluator.h"

int Evaluator::evaluate(const ChessBoard &board) {

    int gamePhase = 0;
    int whiteTableMidScore = 0;
    int whiteTableEndScore = 0;
    int blackTableMidScore = 0;
    int blackTableEndScore = 0;

    for (const Piece &piece: board.whitePieces) {
        short index = piece.type - 1;
        whiteTableMidScore += mg_value[index] + mg_pesto_table[index][piece.position];
        whiteTableEndScore += eg_value[index] + eg_pesto_table[index][piece.position];
        gamePhase += gamephaseInc[index];
    }
    for (const short &pos: board.whitePawns) {
        whiteTableMidScore += mg_value[0] + mg_pesto_table[0][pos];
        whiteTableEndScore += eg_value[0] + eg_pesto_table[0][pos];
    }
    for (const Piece &piece: board.blackPieces) {
        short index = piece.type - 1;
        short position = flipPosition(piece.position);
        blackTableMidScore += mg_value[index] + mg_pesto_table[index][position];
        blackTableEndScore += eg_value[index] + eg_pesto_table[index][position];
        gamePhase += gamephaseInc[index];
    }
    for (const short &pos: board.blackPawns) {
        short position = flipPosition(pos);
        blackTableMidScore += mg_value[0] + mg_pesto_table[0][position];
        blackTableEndScore += eg_value[0] + eg_pesto_table[0][position];
    }

    int midGameScore = (board.sideToMove == WHITE) ? whiteTableMidScore - blackTableMidScore : blackTableMidScore -
                                                                                               whiteTableMidScore;
    int endGameScore = (board.sideToMove == WHITE) ? whiteTableEndScore - blackTableEndScore : blackTableEndScore -
                                                                                               whiteTableEndScore;

    int midGamePhase = (gamePhase > 24) ? 24 : gamePhase;
    int endGamePhase = 24 - midGamePhase;

    return (midGameScore * midGamePhase + endGameScore * endGamePhase) / 24;
}

short Evaluator::flipPosition(short pos) {
    short file = pos % 8;
    short rank = pos / 8;

    return (7 - rank) * 8 + file;
}
