#include <iostream>
#include "Bitboard.h"
#include "MoveGenerator/PieceAttacks/KnightAttack.hpp"
#include "MoveGenerator/PieceAttacks/PawnAttack.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp"
#include "MoveGenerator/PieceRelevantFieldsMask/QueenRelevantMoveMask.hpp"


int main() {

    BitBoard board1 = BishopRelevantMoveMask::generateRelevantFieldsMask(1);
    BitBoard board2 = RookRelevantMoveMask::generateRelevantFieldsMask(1);
    BitBoard board3 = QueenRelevantMoveMask::generateRelevantFieldsMask(1);


    Bitboards::print_bb(board1);

    std::cout<<"\n\n\n================================================\n\n\n";

    Bitboards::print_bb(board2);


    std::cout<<"\n\n\n================================================\n\n\n";

    Bitboards::print_bb(PawnAttack::generateWhitePawnAttacks(1));


    return 0;
}
