#pragma once
#include <cstdint>

namespace Move {
    enum MoveType : uint16_t { MT_NORMAL = 0, MT_CASTLE = 1, MT_ENPASSANT = 2, MT_PROMOTION = 3 };

    enum Promo : uint16_t { PR_KNIGHT = 0, PR_BISHOP = 1, PR_ROOK = 2, PR_QUEEN = 3 };

    struct MoveList {
        std::vector<uint16_t> m;
        void push(const uint16_t x) { m.push_back(x); }
    };

    /**
     * Encode move to 16bit format
     * @param from start position index
     * @param to target position index
     * @param moveType move type
     * @param promotion promotion type
     * @return uint16_t encoded move
     */
    static uint16_t encodeMove(
        const int &from,
        const int &to,
        const MoveType &moveType = MT_NORMAL,
        const Promo &promotion = PR_QUEEN
    ) {
        return static_cast<uint16_t>((moveType << 14) | (promotion << 12) | (from << 6) | to);
    }

    inline uint8_t moveFrom(const uint16_t &m) { return (m >> 6) & 0x3F; }
    inline uint8_t moveTo(const uint16_t &m) { return m & 0x3F; }

    /**
     *
     * @param from from position index
     * @param moves Bitboard with all pseudo-legal moves
     * @param moveList Queue with moves
     */
    static void extractMovesFromBitboard(
        const uint8_t &from,
        BitBoard moves,
        MoveList &moveList
    ) {
        while (moves) {
            int to = Bitboards::lsb_index(moves);
            Bitboards::pop_lsb(moves);
            moveList.push(encodeMove(from, to, MT_NORMAL));
        }
    }
};
