#pragma once
#include "BishopRelevantMoveMask.hpp"
#include "RookRelevantMoveMask.hpp"
#include "../../Bitboard.h"

class QueenRelevantMoveMask {
public:
    static BitBoard generateRelevantFieldsMask(const uint8_t &position) {
        return BishopRelevantMoveMask::generateRelevantFieldsMask(position) |
               RookRelevantMoveMask::generateRelevantFieldsMask(position);
    }
};
