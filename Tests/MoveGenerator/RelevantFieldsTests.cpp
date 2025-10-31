#include <cstdint>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include "../../MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "../../MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp"

#include "../../Bitboard.h"

TEST_CASE("Rook relevant mask excludes edge squares (d4)", "[mask][rook]") {
    const int sq = Bitboards::getPos(3, 3);
    BitBoard m = RookRelevantMoveMask::generateRelevantFieldsMask(sq);

    // Na pliku D: d5,d6,d7 ∈ mask; d8 ∉ mask
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(3,4)))); // d5
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(3,5)))); // d6
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(3,6)))); // d7
    REQUIRE(!(m & Bitboards::bit(Bitboards::getPos(3,7)))); // d8 (brzeg) – NIErelevant

    // Na ranku 4: e4,f4,g4 ∈; h4 ∉
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(4,3))));
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(5,3))));
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(6,3))));
    REQUIRE(!(m & Bitboards::bit(Bitboards::getPos(7,3))));
}


TEST_CASE("Bishop relevant mask excludes edge squares (d4)", "[mask][bishop]") {
    const int sq = Bitboards::getPos(3, 3); // d4
    BitBoard m = BishopRelevantMoveMask::generateRelevantFieldsMask(sq);

    // NE: e5,f6,g7 ∈; h8 ∉
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(4,4))));
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(5,5))));
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(6,6))));
    REQUIRE(!(m & Bitboards::bit(Bitboards::getPos(7,7))));

    // SW: c3,b2 ∈; a1 ∉
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(2,2))));
    REQUIRE((m & Bitboards::bit(Bitboards::getPos(1,1))));
    REQUIRE(!(m & Bitboards::bit(Bitboards::getPos(0,0))));
}
