#include <iostream>
#include "Bitboard.h"
#include "MoveGenerator/PieceAttacks/KnightAttack.hpp"
#include "MoveGenerator/PieceAttacks/PawnAttack.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/QueenRelevantMoveMask.hpp"
#include "MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"


int main() {
    // BitBoard board1 = BishopRelevantMoveMask::generateRelevantFieldsMask(1);
    // BitBoard board2 = RookRelevantMoveMask::generateRelevantFieldsMask(1);
    // BitBoard board3 = QueenRelevantMoveMask::generateRelevantFieldsMask(1);
    //
    //
    // Bitboards::print_bb(board1);
    //
    // std::cout<<"\n\n\n================================================\n\n\n";
    //
    // Bitboards::print_bb(board2);
    //
    //
    // std::cout<<"\n\n\n================================================\n\n\n";
    //
    // Bitboards::print_bb(PawnAttack::generateWhitePawnAttacks(1));
    //


    const auto precomputed = PreComputedMovesGenerator::generate();

    // --- 1) PRZYKŁADOWA POZYCJA ---
    // Indeksowanie: a1=0 ... h8=63; d4 = file=3, rank=3 -> 3 + 3*8 = 27
    const int sqR = 27; // biała wieża na d4
    BitBoard white = 0, black = 0;

    // Blokery: ustawmy po jednym w każdą stronę od d4:
    // d6 (d-file, rank 5) -> 3 + 5*8 = 43 (czarny)
    // f4 (file 5, rank 3) -> 29 (czarny)
    // d2 (file 3, rank 1) -> 11 (czarny)
    // b4 (file 1, rank 3) -> 25 (biały, żeby pokazać odfiltrowanie własnych)
    black |= Bitboards::bit(59) | Bitboards::bit(29) | Bitboards::bit(11);
    white |= Bitboards::bit(25) | Bitboards::bit(27); // nasza wieża + biały bloker na b4
    BitBoard occAll = white | black;

    BitBoard mask = RookRelevantMoveMask::generateRelevantFieldsMask(sqR);

    // --- 3) RUNTIME: relevant, indeks, lookup ---
    BitBoard relevant = occAll & mask;
    const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

    BitBoard attacks = precomputed->rook[idx][64-sqR];
    // BitBoard legalPseudo = attacks & ~white;

    // --- 4) WIZUALIZACJA ---
    std::cout << "Mask (relevant squares for rook @ d4):\n";
    Bitboards::print_bb(mask);
    std::cout << "\nOccupancy (all pieces):\n";
    Bitboards::print_bb(occAll);
    std::cout << "\nRook attacks from d4 (pseudo-legal, after filtering own pieces):\n";
    Bitboards::print_bb(attacks);

    return 0;
}
