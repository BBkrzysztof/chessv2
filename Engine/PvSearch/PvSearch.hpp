#pragma once
#include <memory>

#include "../../Board/Board.hpp"
#include "../QuickSearch/QuickSearch.hpp"
#include "../TranspositionTable/TranspositionTable.hpp"
#include "../TranspositionTable/Zobrist.hpp"

class PvSearch {
public:
    static int search(
        const std::unique_ptr<Board> &board,
        int alpha,
        const int beta,
        const int &depth,
        const int ply,
        TranspositionTable &tt,
        const Zobrist &zob
    ) {
        if (depth == 1) return -QuickSearch::search(board, alpha, beta, ply);
        const auto us = board->side;

        const int alpha0 = alpha;

        // === TT PROBE (przed generacją ruchów) ===
        const uint64_t key = zob.computeKey(*board);
        {
            auto hit = tt.probe(key, depth);
            if (hit.hit) {
                const int tts = Evaluation::from_tt_score(hit.score, ply);
                if (hit.flag == TTFlag::EXACT) {
                    return tts;
                }
                if (hit.flag == TTFlag::LOWER && tts >= beta) {
                    return tts;
                }
                if (hit.flag == TTFlag::UPPER && tts <= alpha) {
                    return tts;
                }
            }
        }


        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return -Evaluation::MATE + ply;
            }
            return 0;
        }

        int best = Evaluation::NEG_INF;
        Move::Move bestMove = 0;
        bool foundLegal = false;

        for (size_t i = 0; i < m.size(); i++) {
            auto move = m[i];
            const auto child = MoveExecutor::executeMove(board, move);
            if (MoveExecutor::isCheck(child, us)) {
                continue;
            }

            foundLegal = true;
            const int score = -search(child, -beta, -alpha, depth - 1, ply + 1, tt, zob);

            if (score > best) {
                best = score;
                bestMove = move;
            }

            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    break;
                }
            }
        }

        if (!foundLegal) {
            if (MoveExecutor::isCheck(board, us)) {
                return Evaluation::MATE - (ply * 10);
            }
            return 0;
        }

        auto flag = TTFlag::EXACT;
        if (best <= alpha0) flag = TTFlag::UPPER;
        else if (best >= beta) flag = TTFlag::LOWER;
        const int stored = Evaluation::to_tt_score(best, ply);
        const auto packedMove = bestMove;
        tt.store(key, static_cast<uint8_t>(depth), stored, flag, packedMove);

        return alpha;
    }
};
