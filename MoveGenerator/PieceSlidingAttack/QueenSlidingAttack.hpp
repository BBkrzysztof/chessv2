#pragma once
#include "BishopSlidingAttack.hpp"
#include "RookSlidingAttack.hpp"
#include "../../Bitboard.h"

class QueenSlidingAttack {
public:
    static BitBoard generateSlidingAttacks(const uint8_t &position, const BitBoard &occupancy) {
        return RookSlidingAttack::generateSlidingAttacks(position, occupancy) |
               BishopSlidingAttack::generateSlidingAttacks(position, occupancy);
    }
};
