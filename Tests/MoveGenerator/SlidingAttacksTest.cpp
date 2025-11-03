#include <cstdint>
#include <vector>
#include <array>
#include <limits>
#include <algorithm>

#include <catch2/catch_test_macros.hpp>
#include "../../MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "../../MoveGenerator/PreComputedMoves/MagicBoardIndexGenerator.hpp"
#include "../../MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"
#include "../../MoveGenerator/Move/Move.hpp"


using namespace Bitboards;
using namespace Move;

const auto precomputed = PreComputedMovesGenerator::instance();

TEST_CASE("Sliding Precomputed Rook Attacks", "[Rook sliding Attacks]") {
    const int sqR = 27; // white rook on d4
    BitBoard white = 0, black = 0;

    // d6 (d-file, rank 5) -> 3 + 5*8 = 43 (black)
    // f4 (file 5, rank 3) -> 29 (black)
    // d2 (file 3, rank 1) -> 11 (black)
    // b4 (file 1, rank 3) -> 25 (White to filter out)
    black |= Bitboards::bit(43) | Bitboards::bit(29) | Bitboards::bit(11);
    white |= Bitboards::bit(25) | Bitboards::bit(27);
    BitBoard occAll = white | black;

    BitBoard mask = RookRelevantMoveMask::generateRelevantFieldsMask(sqR);
    BitBoard relevant = occAll & mask;
    const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

    BitBoard attacks = precomputed.rook[sqR][idx];
    BitBoard legalPseudo = attacks & ~white;

    REQUIRE(legalPseudo == 0x80834080800);

    MoveList movesQueue;

    extractMovesFromBitboard(
        sqR,
        legalPseudo,
        movesQueue
    );

    REQUIRE(movesQueue.m.size() == 7);
}

TEST_CASE("Sliding Precomputed Bishop Attacks", "[Bishop sliding Attacks]") {
    const int sqR = 27; // white rook on d4
    BitBoard white = 0, black = 0;

    // d6 (d-file, rank 5) -> 3 + 5*8 = 43 (black)
    // f4 (file 5, rank 3) -> 29 (black)
    // d2 (file 3, rank 1) -> 11 (black)
    // b4 (file 1, rank 3) -> 25 (White to filter out)
    black |= Bitboards::bit(43) | Bitboards::bit(29) | Bitboards::bit(11);
    white |= Bitboards::bit(25) | Bitboards::bit(27);
    BitBoard occAll = white | black;

    BitBoard mask = BishopRelevantMoveMask::generateRelevantFieldsMask(sqR);
    BitBoard relevant = occAll & mask;
    const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

    BitBoard attacks = precomputed.bishop[sqR][idx];
    BitBoard legalPseudo = attacks & ~white;

    REQUIRE(legalPseudo == 0x8041221400142241);

    MoveList movesQueue;

    extractMovesFromBitboard(
        sqR,
        legalPseudo,
        movesQueue
    );

    REQUIRE(movesQueue.m.size() == 13);
}
