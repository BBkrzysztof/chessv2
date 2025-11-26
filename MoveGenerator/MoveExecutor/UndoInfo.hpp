#pragma once
#include "../../Bitboard.h"

struct UndoInfo {
    BitBoard zobristBefore;
    BitBoard zobristAfter;

    uint8_t castleBefore;
    int epBefore;
    uint16_t halfMoveBefore;
    uint16_t fullMoveBefore;
    int8_t captured;
};
