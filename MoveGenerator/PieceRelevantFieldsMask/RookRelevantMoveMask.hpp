#pragma once
#include "../../Bitboard.h"


class RookRelevantMoveMask {
public:
    BitBoard static generateRelevantFieldsMask(const uint8_t &position) {
        BitBoard result = 0;

        const uint8_t row = Bitboards::row_of(position);
        const uint8_t column = Bitboards::column_of(position);

        const int8_t deltaRow[4] = {+1, -1, 0, 0};
        const int8_t deltaColumn[4] = {0, 0, +1, -1};

        for (int dir = 0; dir < 4; dir++) {
            int rowPos = row + deltaRow[dir];
            int columnPos = column + deltaColumn[dir];

            while (rowPos >= 0 && rowPos <= 7 && columnPos >= 0 && columnPos <= 7) {
                const int tempRowPos = rowPos + deltaRow[dir];
                const int tempColumnPos = columnPos + deltaColumn[dir];

                if (tempRowPos < 0 || tempRowPos > 7 || tempColumnPos < 0 || tempColumnPos > 7) break;

                result |= 1ull << (rowPos * 8 + columnPos);

                rowPos = tempRowPos;
                columnPos = tempColumnPos;
            }
        }

        return result;
    }
};
