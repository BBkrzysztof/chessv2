#pragma once
#include <cstdint>
#include <array>

#include "../Bitboard.h"

class Zobrist {
public:
    static constexpr int COLORS = 2;
    static constexpr int PIECE_TYPES = 6;
    static constexpr int PIECE_INDEX = COLORS * PIECE_TYPES;
    static constexpr int SQUARES = 64;

    static Zobrist &instance() {
        static auto instance = [] {
            return Zobrist();
        }();

        return instance;
    }

    explicit Zobrist(const BitBoard seed = 0x9e3779b97f4a7c15ULL) {
        uint64_t s = seed;
        for (auto &p: pieceRnd)
            for (unsigned long long &sq: p)
                sq = splitmix64_seeded(s);

        for (unsigned long long &i: castlingRnd) i = splitmix64_seeded(s);
        for (unsigned long long &f: epFileRnd) f = splitmix64_seeded(s);
        sideRnd = splitmix64_seeded(s);
    }

    [[nodiscard]] BitBoard computeKeyFromArrays(
        const std::array<std::array<BitBoard, PIECE_TYPES>, COLORS> &pieces,
        const int side,
        const uint8_t castlingRights,
        const int epSquare
    ) const {
        uint64_t key = 0ULL;

        for (int color = 0; color < COLORS; ++color) {
            for (int pt = 0; pt < PIECE_TYPES; ++pt) {
                BitBoard bb = pieces[color][pt];
                if (!bb) continue;
                const int pIndex = pieceIndexFrom(color, pt);
                while (bb) {
                    const int sq = pop_lsb(bb);
                    key ^= pieceRnd[pIndex][sq];
                }
            }
        }

        key ^= castlingRnd[castlingRights & 0x0F];

        if (epSquare >= 0 && epSquare < 64) {
            const int file = epSquare & 7;
            key ^= epFileRnd[file];
        }

        if (side != 0) key ^= sideRnd;
        return key;
    }

    template<typename BoardT>
    BitBoard computeKey(const BoardT &board) const {
        std::array<std::array<BitBoard, PIECE_TYPES>, COLORS> pcs{};
        for (int c = 0; c < COLORS; ++c)
            for (int pt = 0; pt < PIECE_TYPES; ++pt)
                pcs[c][pt] = static_cast<BitBoard>(board.pieces[c][pt]);

        return computeKeyFromArrays(
            pcs,
            static_cast<int>(board.side),
            static_cast<uint8_t>(board.castle),
            static_cast<int>(board.ep)
        );
    }

    static constexpr int pieceIndexFrom(const int color, const int pieceType) {
        return color * PIECE_TYPES + pieceType;
    }

    BitBoard pieceRnd[PIECE_INDEX][SQUARES]{};

private:
    BitBoard castlingRnd[16]{};
    BitBoard epFileRnd[8]{};
    BitBoard sideRnd{};


    static int pop_lsb(BitBoard &bb) {
        const int idx = __builtin_ctzll(bb);
        bb &= (bb - 1);
        return idx;
    }

    static uint64_t splitmix64_seeded(uint64_t &x) {
        uint64_t z = (x += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};
