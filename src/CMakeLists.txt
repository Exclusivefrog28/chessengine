project(ChessEngine)

set(HEADER_FILES
        ChessBoard.h
        CLI.h
        EvaluationValues.h
        Evaluator.h
        HashCodes.h
        Move.h
        MoveGenerator.h
        Piece.h
        Search.h
        TranspositionTable.h
        Util.h
)

set(SOURCE_FILES
        ChessBoard.cpp
        CLI.cpp
        Evaluator.cpp
        MoveGenerator.cpp
        Search.cpp
        TranspositionTable.cpp
        Logger.cpp
        Logger.h
)

add_library(src STATIC ${SOURCE_FILES} ${HEADER_FILES})