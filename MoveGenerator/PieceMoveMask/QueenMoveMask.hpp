#pragma once
#include "BishopMoveMask.hpp"
#include "RookMoveMask.hpp"
#include "../../Bitboard.h"

class QueenMoveMask {
    public:
    static BitBoard generate(const uint8_t& position) {
        return BishopMoveMask::generate(position) | RookMoveMask::generate(position);
    }
};
