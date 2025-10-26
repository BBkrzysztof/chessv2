#pragma once
#include "../../Bitboard.h"

class KnightAttack {



public:
    BitBoard static generateKnightAttacks(const uint8_t& position) {
        const BitBoard pos = Bitboards::bit(position);
        BitBoard result = 0;

        result |= (pos << 17) & ~Bitboards::FILE_A;
        result |= (pos << 15) & ~Bitboards::FILE_H;
        result |= (pos << 10) & ~Bitboards::FILE_AB;
        result |= (pos << 6 ) & ~Bitboards::FILE_GH;
        result |= (pos >> 17) & ~Bitboards::FILE_H;
        result |= (pos >> 15) & ~Bitboards::FILE_A;
        result |= (pos >> 10) & ~Bitboards::FILE_GH;
        result |= (pos >> 6 ) & ~Bitboards::FILE_AB;

        return result;
    }
};
