#pragma once
#include "../../Bitboard.h"

class PawnAttack {
public:
    BitBoard static generateWhitePawnAttacks(const uint8_t position) {
        const BitBoard pos = Bitboards::bit(position);
        BitBoard result = 0;

        result |= ((pos << 9) & ~Bitboards::FILE_H);
        result |= ((pos << 7) & ~Bitboards::FILE_A);

        return result;
    }

    BitBoard static generateBlackPawnAttacks(const uint8_t position) {
        const BitBoard pos = Bitboards::bit(position);
        BitBoard result = 0;

        result |= ((pos >> 7) & ~Bitboards::FILE_H);
        result |= ((pos >> 9) & ~Bitboards::FILE_A);

        return result;
    }
};
