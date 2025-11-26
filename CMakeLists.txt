cmake_minimum_required(VERSION 3.24)
project(chess)
include(FetchContent)

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
        Engine/QuickSearch/QuickSearch.hpp
        Engine/PvSearch/PvSearch.hpp
        Engine/Engine.hpp
        Engine/TranspositionTable/TranspositionTable.hpp
        Board/Zobrist.hpp
        Engine/AlphaBeta/AlphaBeta.hpp
        MoveGenerator/MoveExecutor/UndoInfo.hpp)

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

file(GLOB_RECURSE TEST_SOURCES
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/Tests/**/*.cpp"
)

add_executable(tests ${TEST_SOURCES}
        Tests/Engine/ThreadPool/ThreadPool.cpp
        Tests/Engine/TestUtils.hpp)


target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/../include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
enable_testing()
add_test(NAME unit_tests COMMAND tests)

# Optymalizacje jak chcesz (tu O3 + native)
target_compile_options(chess PRIVATE -O3 -march=native)

