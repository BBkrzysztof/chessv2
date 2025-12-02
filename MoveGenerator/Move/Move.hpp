#pragma once
#include <cstdint>
#include <iostream>

namespace Move {

    typedef uint16_t Move;

    enum MoveType : Move { MT_NORMAL = 0, MT_CASTLE = 1, MT_ENPASSANT = 2, MT_PROMOTION = 3 };

    enum Promo : Move { PR_KNIGHT = 0, PR_BISHOP = 1, PR_ROOK = 2, PR_QUEEN = 3 };

    struct MoveList {
        MoveList() {
            m.reserve(256);
        }
        std::vector<Move> m;
        void push(const Move &x) { m.push_back(x); }
    };

    /**
     * Encode move to 16bit format
     * @param from start position index
     * @param to target position index
     * @param moveType move type
     * @param promotion promotion type
     * @return Move encoded move
     */
    static Move encodeMove(
        const int &from,
        const int &to,
        const MoveType &moveType = MT_NORMAL,
        const Promo &promotion = PR_QUEEN
    ) {
        return static_cast<Move>((moveType << 14) | (promotion << 12) | (from << 6) | to);
    }

    inline uint8_t moveFrom(const Move &m) { return (m >> 6) & 0x3F; }
    inline uint8_t moveTo(const Move &m) { return m & 0x3F; }
    inline MoveType moveType(const Move &m) { return static_cast<MoveType>((m >> 14) & 0x3); }
    inline Promo movePromo(const Move &m) { return static_cast<Promo>((m >> 12) & 0x3); }
    /**
     *
     * @param from from position index
     * @param moves Bitboard with all pseudo-legal moves
     * @param moveList Queue with moves
     */
    static void extractMovesFromBitboard(
        const uint8_t &from,
        BitBoard &moves,
        MoveList &moveList
    ) {
        while (moves) {
            int to = Bitboards::lsb_index(moves);
            Bitboards::pop_lsb(moves);
            moveList.push(encodeMove(from, to, MT_NORMAL));
        }
    }
};
