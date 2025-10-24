#pragma once
#include <cstdint>
#include <iostream>

typedef uint64_t BitBoard;

namespace Bitboards {
    static BitBoard bit(const uint8_t& sq){ return 1ULL<<sq; }
    static uint8_t column_of(const uint8_t& sq){ return sq & 7; }
    static uint8_t row_of(const uint8_t& sq){ return sq >> 3; }
    static int popCount64(const BitBoard& x){ return __builtin_popcountll(x); }

    static void print_bb(const BitBoard& b){
        for(int r=7; r>=0; --r){
            for(int f=0; f<8; ++f){
                int sq=r*8+f;
                std::cout<<( (b>>sq)&1ULL ? " X" : " ." );
            }
            std::cout<<"\n";
        }
    }
}

