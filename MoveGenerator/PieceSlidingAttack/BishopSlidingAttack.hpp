#pragma once
#include "../../Bitboard.h"


class BishopSlidingAttack {
public:

    BitBoard static generateSlidingAttacks(const uint8_t &position, const BitBoard &occupancy) {
        BitBoard result = 0;

        const uint8_t row = Bitboards::row_of(position);
        const uint8_t column = Bitboards::column_of(position);

        const int8_t deltaRow[4] = {+1, +1, -1, -1};
        const int8_t deltaColumn[4] = {+1, -1, +1, -1};

        for (int dir = 0; dir < 4; dir++) {
            int rowPos = row + deltaRow[dir];
            int columnPos = column + deltaColumn[dir];

            while (rowPos >= 0 && rowPos <= 7 && columnPos >= 0 && columnPos <= 7) {
                const int tempRowPos = rowPos + deltaRow[dir];
                const int tempColumnPos = columnPos + deltaColumn[dir];

                const int sq = rowPos * 8 + columnPos;
                result |= Bitboards::bit(sq);

                rowPos = tempRowPos;
                columnPos = tempColumnPos;

                if (occupancy & Bitboards::bit(sq)) break;
            }
        }

        return result;
    }
};
