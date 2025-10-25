#include <cstdint>
#include <vector>
#include <array>
#include <limits>
#include <algorithm>

#include <catch2/catch_test_macros.hpp>
#include "utils.hpp"
#include "../MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "../MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp"
#include "../MoveGenerator/PieceRelevantFieldsMask/QueenRelevantMoveMask.hpp"

#include "../MoveGenerator/PieceSlidingAttack/BishopSlidingAttack.hpp"
#include "../MoveGenerator/PieceSlidingAttack/RookSlidingAttack.hpp"
#include "../MoveGenerator/PieceSlidingAttack/QueenSlidingAttack.hpp"

using namespace Bitboards;
using namespace Tests;

TEST_CASE("Rook: single blocker north stops exactly at blocker", "[rook][blockers]") {
    // d4 -> bloker na d6 (północ; r+)
    const auto sq = getPos(3, 3); // d4
    const int r0 = row_of(sq), c0 = column_of(sq);

    const int br = 5, bc = 3; // d6
    const BitBoard occAll = bit(getPos(bc, br));

    const BitBoard mask = RookRelevantMoveMask::generateRelevantFieldsMask(sq);
    const BitBoard sub = occAll & mask;
    const BitBoard att = RookSlidingAttack::generateSlidingAttacks(sq, sub);

    // Pola do krawędzi na północ od d4
    const BitBoard north_to_edge = ray_to_edge(r0, c0, +1, 0);
    const BitBoard north_beyond = beyond_blocker(br, bc, +1, 0);


    // print_bb(bit(expected));
    std::cout << "\n==================" << std::endl;
    print_bb(att);

    // 1) bloker jest w atakach
    REQUIRE(att & bit(getPos(bc,br)));

    // 2) nic za blokerem
    REQUIRE((att & north_beyond) == 0ULL);

    // 3) pozostałe kierunki bez zmian (brak blokerów) — ataki zawierają całą resztę promieni
    const BitBoard others = (ray_to_edge(r0, c0, -1, 0) // south
                             | ray_to_edge(r0, c0, 0, +1) // east
                             | ray_to_edge(r0, c0, 0, -1)); // west
    // z północy zostaje część do blokera włącznie:
    const BitBoard north_until_blocker = (north_to_edge ^ north_beyond);
    const BitBoard expected = others | north_until_blocker;




    REQUIRE(att == expected);
}

TEST_CASE("Rook: nearest blocker on a ray decides (two blockers north)", "[rook][blockers]") {
    // d4 -> blokery na d5 i d7; najbliższy to d5
    const auto sq = getPos(3, 3); // d4
    const int r0 = row_of(sq), c0 = column_of(sq);
    const BitBoard occAll = bit(getPos(3, 4)) | bit(getPos(3, 6)); // d5, d7

    const BitBoard sub = occAll & RookRelevantMoveMask::generateRelevantFieldsMask(sq);
    const BitBoard att = RookSlidingAttack::generateSlidingAttacks(sq, sub);

    // Część północna powinna skończyć się na d5 (d7 i dalej wykluczone)
    const BitBoard north_to_edge = ray_to_edge(r0, c0, +1, 0);
    const BitBoard beyond_d5 = beyond_blocker(4, 3, +1, 0); // za d5
    const BitBoard north_until_d5 = north_to_edge ^ beyond_d5;

    // Oczekiwane: pozostałe trzy kierunki pełne + północ do d5 włącznie
    const BitBoard others = (ray_to_edge(r0, c0, -1, 0)
                             | ray_to_edge(r0, c0, 0, +1)
                             | ray_to_edge(r0, c0, 0, -1));
    const BitBoard expected = others | north_until_d5;
    REQUIRE(att == expected);
}

TEST_CASE("Rook: adjacent blocker includes only that square in that direction", "[rook][blockers]") {
    // d4 -> bloker na e4 (tuż obok na wschód)
    const auto sq = getPos(3, 3);
    const int r0 = row_of(sq), c0 = column_of(sq);

    const BitBoard occAll = bit(getPos(4, 3)); // e4
    const BitBoard att = RookSlidingAttack::generateSlidingAttacks(
        sq, occAll & RookRelevantMoveMask::generateRelevantFieldsMask(sq));

    // Wschód: tylko e4; żadne pole dalej
    const BitBoard east_beyond = beyond_blocker(3, 4, 0, +1);
    REQUIRE((att & bit(getPos(4,3))) != 0ULL);
    REQUIRE((att & east_beyond) == 0ULL);

    // Inne kierunki — pełne promienie (brak blokerów)
    const BitBoard others = (ray_to_edge(r0, c0, +1, 0) | ray_to_edge(r0, c0, -1, 0) | ray_to_edge(r0, c0, 0, -1));
    REQUIRE((att & others) == others);
}

// ─────────────────────────────────────────────────────────────────────────────
// GONIEC — bloker na przekątnej
// ─────────────────────────────────────────────────────────────────────────────
TEST_CASE("Bishop: diagonal blocker stops at blocker (NE)", "[bishop][blockers]") {
    // d4 -> bloker na f6 (NE)
    const auto sq = getPos(3, 3);
    const int r0 = row_of(sq), c0 = column_of(sq);

    const int br = 5, bc = 5; // f6
    const BitBoard occAll = bit(getPos(bc, br));
    const BitBoard att = BishopSlidingAttack::generateSlidingAttacks(
        sq, occAll & BishopRelevantMoveMask::generateRelevantFieldsMask(sq));

    const BitBoard ne_to_edge = ray_to_edge(r0, c0, +1, +1);
    const BitBoard ne_beyond = beyond_blocker(br, bc, +1, +1);

    REQUIRE((att & bit(getPos(bc,br))) != 0ULL); // bloker w atakach
    REQUIRE((att & ne_beyond) == 0ULL); // nic dalej

    // pozostałe 3 diagonale pełne
    const BitBoard others = (ray_to_edge(r0, c0, +1, -1) | ray_to_edge(r0, c0, -1, +1) | ray_to_edge(r0, c0, -1, -1));
    const BitBoard expected = (ne_to_edge ^ ne_beyond) | others;
    REQUIRE(att == expected);
}

TEST_CASE("Bishop: nearest diagonal blocker wins (two blockers NE)", "[bishop][blockers]") {
    // d4 -> blokery NE: e5 i h8; zatrzymujemy na e5
    const auto sq = getPos(3, 3);
    const int r0 = row_of(sq), c0 = column_of(sq);

    const BitBoard occAll = bit(getPos(4, 4)) | bit(getPos(7, 7)); // e5, h8
    const BitBoard att = BishopSlidingAttack::generateSlidingAttacks(
        sq, occAll & BishopRelevantMoveMask::generateRelevantFieldsMask(sq));

    const BitBoard ne_to_edge = ray_to_edge(r0, c0, +1, +1);
    const BitBoard beyond_e5 = beyond_blocker(4, 4, +1, +1);
    const BitBoard ne_until_e5 = ne_to_edge ^ beyond_e5;

    // pozostałe diagonale pełne
    const BitBoard others = (ray_to_edge(r0, c0, +1, -1) | ray_to_edge(r0, c0, -1, +1) | ray_to_edge(r0, c0, -1, -1));
    REQUIRE(att == (others | ne_until_e5));
}

// ─────────────────────────────────────────────────────────────────────────────
// HETMAN — niezależność kierunków (ortogonalne vs diagonalne)
// ─────────────────────────────────────────────────────────────────────────────
TEST_CASE("Queen (R|B): blockers on different rays are independent", "[queen][blockers]") {
    // d4 -> bloker na d6 (północ) i bloker na f6 (NE)
    const auto sq = getPos(3, 3);
    const int r0 = row_of(sq), c0 = column_of(sq);

    const BitBoard occAll = bit(getPos(3, 5)) | bit(getPos(5, 5)); // d6, f6

    const BitBoard qAtt = QueenSlidingAttack::generateSlidingAttacks(sq, occAll);

    // Oczekujemy, że północ rooka zatrzyma się na d6,
    // a NE gońca zatrzyma się na f6 — niezależnie od siebie
    const BitBoard north_to_edge = ray_to_edge(r0, c0, +1, 0);
    const BitBoard north_beyond = beyond_blocker(5, 3, +1, 0);
    const BitBoard ne_to_edge = ray_to_edge(r0, c0, +1, +1);
    const BitBoard ne_beyond = beyond_blocker(5, 5, +1, +1);

    const BitBoard expected =
            // pełne pozostałe promienie rooka i bishopa
            (ray_to_edge(r0, c0, -1, 0) | ray_to_edge(r0, c0, 0, +1) | ray_to_edge(r0, c0, 0, -1)) |
            (ray_to_edge(r0, c0, +1, -1) | ray_to_edge(r0, c0, -1, +1) | ray_to_edge(r0, c0, -1, -1)) |
            // północ i NE ucięte na blokerach
            ((north_to_edge ^ north_beyond) | (ne_to_edge ^ ne_beyond));

    print_bb(expected);
    std::cout << "\n==================" << std::endl;
    print_bb(qAtt);


    REQUIRE(qAtt == expected);
}
