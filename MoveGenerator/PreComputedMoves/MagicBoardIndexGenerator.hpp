#include "../../Bitboard.h"
#if defined(__x86_64__) || defined(_M_X64)
#if defined(__BMI2__)
#include <immintrin.h>
#endif
#endif


class MagicBoardIndexGenerator {
    static bool cpuHasBmi2() {
#if defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
        // dostępne od GCC/Clang: wykrywa *runtime* cechę CPU
        return __builtin_cpu_supports("bmi2");
#else
        return false;
#endif
    }

public:
    static BitBoard getId(BitBoard src, BitBoard mask) {
#if defined(__BMI2__)
        if (MagicBoardIndexGenerator::cpuHasBmi2()) {
            return _pext_u64(src, mask);
        }
#endif
        const BitBoard magic = 0xA180022080400230ULL;
        const int k = Bitboards::popCount64(mask);
        return (BitBoard) (src & magic) >> (64 - k);
    }
};
