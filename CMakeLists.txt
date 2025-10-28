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
)

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

add_executable(tests
        Tests/RelevantFieldsTests.cpp
        Tests/SlidingAttacksTest.cpp
)

target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/../include)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
enable_testing()
add_test(NAME unit_tests COMMAND tests)

target_compile_options(chess PRIVATE -O3 -march=native)
