#pragma once
#include <cstdint>
#include <array>

class Zobrist {
public:
    using BitBoard = uint64_t;

    // Stałe: 2 kolory, 6 figur, 64 pola.
    static constexpr int COLORS = 2;
    static constexpr int PIECE_TYPES = 6;                 // PAWN=0..KING=5
    static constexpr int PIECE_INDEX = COLORS*PIECE_TYPES; // 12
    static constexpr int SQUARES = 64;

    // Minimalny konstruktor – wypełnia tablice na podstawie ziarna (seed).
    explicit Zobrist(uint64_t seed = 0x9e3779b97f4a7c15ULL) {
        uint64_t s = seed;
        for (int p = 0; p < PIECE_INDEX; ++p)
            for (int sq = 0; sq < SQUARES; ++sq)
                pieceRnd[p][sq] = splitmix64_seeded(s);

        for (int i = 0; i < 16; ++i) castlingRnd[i] = splitmix64_seeded(s);
        for (int f = 0; f < 8;  ++f) epFileRnd[f]  = splitmix64_seeded(s);
        sideRnd = splitmix64_seeded(s);
    }

    uint64_t computeKeyFromArrays(
        const std::array<std::array<BitBoard, PIECE_TYPES>, COLORS> &pieces,
        int side, uint8_t castlingRights, int epSquare
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
            const int file = epSquare & 7; // a..h => 0..7
            key ^= epFileRnd[file];
        }

        if (side != 0) key ^= sideRnd;
        return key;
    }

    // Adapter „z obiektu planszy”.
    // Wymagane pola:
    //   board.pieces[color][pieceType] : uint64_t
    //   board.side : int/enum (0/1)
    //   board.castlingRights : uint8_t
    //   board.epSquare : int (-1 jeśli brak)
    template <typename BoardT>
    uint64_t computeKey(const BoardT &board) const {
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

private:
    // Dane losowe Zobrista.
    uint64_t pieceRnd[PIECE_INDEX][SQUARES]{};
    uint64_t castlingRnd[16]{};
    uint64_t epFileRnd[8]{};
    uint64_t sideRnd{};

    // Mapowanie kolor/figura -> indeks 0..11.
    static constexpr int pieceIndexFrom(int color, int pieceType) {
        return color * PIECE_TYPES + pieceType;
    }

    // Szybkie pop_lsb (czyści najmłodszy bit i zwraca indeks pola 0..63).
    static inline int pop_lsb(BitBoard &bb) {
    #if defined(__GNUG__) || defined(__clang__)
        const int idx = __builtin_ctzll(bb);
        bb &= (bb - 1);
        return idx;
    #else
        int idx = 0;
        BitBoard x = bb;
        while ((x & 1ULL) == 0ULL) { x >>= 1; ++idx; }
        bb &= (bb - 1);
        return idx;
    #endif
    }

    // Minimalny deterministyczny generator do wypełnienia tablic.
    static inline uint64_t splitmix64_seeded(uint64_t &x) {
        uint64_t z = (x += 0x9e3779b97f4a7c15ULL);
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        return z ^ (z >> 31);
    }
};
