#pragma once
#include <memory>

#include "MagicBoardIndexGenerator.hpp"
#include "../../Bitboard.h"
#include "../PieceAttacks/KingAttack.hpp"
#include "../PieceAttacks/KnightAttack.hpp"
#include "../PieceAttacks/PawnAttack.hpp"
#include "../PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp"
#include "../PieceRelevantFieldsMask/QueenRelevantMoveMask.hpp"
#include "../PieceRelevantFieldsMask/RookRelevantMoveMask.hpp"
#include "../PieceSlidingAttack/BishopSlidingAttack.hpp"
#include "../PieceSlidingAttack/QueenSlidingAttack.hpp"
#include "../PieceSlidingAttack/RookSlidingAttack.hpp"

struct PreComputedMoves {
    std::vector<BitBoard> bishop[64];
    std::vector<BitBoard> rook[64];
    std::vector<BitBoard> queen[64];

    BitBoard bishopMask[64]{};
    BitBoard rookMask[64]{};
    BitBoard queenMask[64]{};

    BitBoard knight[64]{};
    BitBoard king[64]{};

    BitBoard whitePawn[64]{};
    BitBoard blackPawn[64]{};
};


class PreComputedMovesGenerator {
    void static precompute(
        const uint8_t &position,
        const BitBoard &mask,
        std::vector<BitBoard> &targetTable,
        BitBoard (*generateAttack)(const uint8_t &, const BitBoard &)
    ) {
        const int bits = Bitboards::popCount64(mask);
        const size_t size = 1u << bits;
        targetTable.assign(size, 0ULL);


        for (const BitBoard subset: Bitboards::allSubsets(mask)) {
            const auto idx = MagicBoardIndexGenerator::getId(subset, mask);

            const BitBoard attacks = generateAttack(position, subset);

            targetTable[idx] = attacks;
        }
    }

public:
    std::unique_ptr<PreComputedMoves> static generate() {
        auto moves = std::make_unique<PreComputedMoves>();

        for (uint8_t i = 0; i < 64; i++) {
            moves->king[i] = KingAttack::generateKingAttacks(i);
            moves->knight[i] = KnightAttack::generateKnightAttacks(i);

            moves->whitePawn[i] = PawnAttack::generateWhitePawnAttacks(i);
            moves->blackPawn[i] = PawnAttack::generateBlackPawnAttacks(i);


            moves->bishopMask[i] = BishopRelevantMoveMask::generateRelevantFieldsMask(i);
            // MagicBoard for bishop
            precompute(
                i,
                moves->bishopMask[i],
                moves->bishop[i],
                BishopSlidingAttack::generateSlidingAttacks
            );

            moves->rookMask[i]=RookRelevantMoveMask::generateRelevantFieldsMask(i);
            // MagicBoard for rook
            precompute(
                i,
                moves->rookMask[i],
                moves->rook[i],
                RookSlidingAttack::generateSlidingAttacks
            );

            moves->queenMask[i]=QueenRelevantMoveMask::generateRelevantFieldsMask(i);
            // MagicBoard for queen
            precompute(
                i,
                moves->queenMask[i],
                moves->queen[i],
                QueenSlidingAttack::generateSlidingAttacks
            );
        }

        return moves;
    };
};
