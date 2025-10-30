#pragma once
#include <cstdint>
#include <iostream>
#include <vector>

#include <cstdint>


typedef uint64_t BitBoard;

namespace Bitboards {
    static constexpr BitBoard FILE_A = 0x0101010101010101ULL;
    static constexpr BitBoard FILE_B = 0x0202020202020202ULL;
    static constexpr BitBoard FILE_G = 0x4040404040404040ULL;
    static constexpr BitBoard FILE_H = 0x8080808080808080ULL;

    static constexpr BitBoard ROW_2 = 0x000000000000FF00ULL;
    static constexpr BitBoard ROW_3 = 0x0000000000FF0000ULL;
    static constexpr BitBoard ROW_4 = 0x00000000FF000000ULL;
    static constexpr BitBoard ROW_7 = 0x00FF000000000000ULL;
    static constexpr BitBoard ROW_8 = 0xFF00000000000000ULL;


    static constexpr BitBoard FILE_AB = FILE_A | FILE_B;
    static constexpr BitBoard FILE_GH = FILE_G | FILE_H;

    static BitBoard bit(const uint8_t &sq) { return 1ULL << sq; }
    static uint8_t column_of(const uint8_t &sq) { return sq & 7; }
    static uint8_t row_of(const uint8_t &sq) { return sq >> 3; }
    static int popCount64(const BitBoard &x) { return __builtin_popcountll(x); }
    static int getPos(const int &row, const int &col) { return row * 8 + col; }
    static  int  lsb_index(const BitBoard& b){ return __builtin_ctzll(b); }
    static  void pop_lsb(BitBoard& b){ b &= (b - 1); }

    static std::vector<BitBoard> allSubsets(const BitBoard& mask) {
        std::vector<BitBoard> v;
        BitBoard s = mask;
        while (true) {
            v.push_back(s);
            if (s == 0) break;
            s = (s - 1) & mask;
        }
        return v;
    }

    static void print_bb(const BitBoard &b) {
        for (int r = 7; r >= 0; --r) {
            for (int f = 0; f < 8; ++f) {
                const int sq = r * 8 + f;
                std::cout << ((b >> sq) & 1ULL ? " X" : " .");
            }
            std::cout << "\n";
        }
        std::cout << std::hex << "\nmask=0x" <<b<<std::endl;

    }
}
