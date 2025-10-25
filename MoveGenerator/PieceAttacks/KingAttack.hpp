#pragma once
#include "../../Bitboard.h"

class KingAttack {
public:
    BitBoard static generateKingAttacks(const uint8_t &position) {
        const BitBoard pos = Bitboards::bit(position);
        BitBoard result = 0;

        result |= (pos << 8);
        result |= (pos >> 8);
        result |= (pos << 1) & ~Bitboards::FILE_A;
        result |= (pos >> 1) & ~Bitboards::FILE_H;
        result |= (pos << 9) & ~Bitboards::FILE_A;
        result |= (pos << 7) & ~Bitboards::FILE_H;
        result |= (pos >> 7) & ~Bitboards::FILE_A;
        result |= (pos >> 9) & ~Bitboards::FILE_H;

        return result;
    }
};
