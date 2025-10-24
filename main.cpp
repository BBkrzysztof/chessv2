#include <iostream>
#include "Bitboard.h"
#include "MoveGenerator/PieceMoveMask/RookMoveMask.hpp"
#include "MoveGenerator/PieceMoveMask/BishopMoveMask.hpp"
#include "MoveGenerator/PieceMoveMask/QueenMoveMask.hpp"


int main() {

    BitBoard board1 = BishopMoveMask::generate(1);
    BitBoard board2 = RookMoveMask::generate(1);
    BitBoard board3 = QueenMoveMask::generate(1);


    Bitboards::print_bb(board1);

    std::cout<<"\n\n\n================================================\n\n\n";

    Bitboards::print_bb(board2);


    std::cout<<"\n\n\n================================================\n\n\n";

    Bitboards::print_bb(board3);


    return 0;
}
