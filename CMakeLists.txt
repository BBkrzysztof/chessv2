cmake_minimum_required(VERSION 3.24)
project(chess)

set(CMAKE_CXX_STANDARD 17)

add_executable(chess
        main.cpp
        MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp
        MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp
        MoveGenerator/PieceAttacks/PawnAttack.hpp
        MoveGenerator/PieceAttacks/KingAttack.hpp
        MoveGenerator/PreComputedMoves/PreComputedMoves.hpp
        MoveGenerator/PreComputedMoves/MagicBoardIndexGenerator.hpp
        MoveGenerator/PreComputedMoves/MagicBoardIndexGenerator.hpp
        MoveGenerator/Move/Move.hpp
        Board/Board.hpp
        Parser/Parser.cpp
        MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp
        MoveGenerator/MoveExecutor/MoveExecutor.hpp
        Engine/Utils/SplitPoint.hpp
        Engine/ThreadPool/ThreadPool.hpp
        Engine/PvSplit/PvSplit.hpp
        Engine/Utils/SearchConfig.hpp
        Engine/Evaluation/Evaluation.hpp
        Engine/Engine.hpp
        Engine/TranspositionTable/TranspositionTable.hpp
        Board/Zobrist.hpp
        Engine/AlphaBeta/AlphaBeta.hpp
        MoveGenerator/MoveExecutor/UndoInfo.hpp)

add_test(NAME unit_tests COMMAND tests)

# Optymalizacje jak chcesz (tu O3 + native)
#target_compile_options(chess PRIVATE -O3 -march=native)
#target_compile_options(chess PRIVATE)
