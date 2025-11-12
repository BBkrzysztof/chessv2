#pragma once

#include "../PvSearch/PvSearch.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include "../Utils/SplitPoint.hpp"
#include "../QuickSearch/QuickSearch.hpp"
#include "../Utils/SearchConfig.hpp"
#include "../TranspositionTable/TranspositionTable.hpp"
#include "../TranspositionTable/Zobrist.hpp"

class PvSplit {
public:
    static int searchPvSplit(
        ThreadPool &pool,
        const SearchConfig &config,
        const std::unique_ptr<Board> &board,
        int alpha,
        const int beta,
        const int depth,
        const int ply,
        TranspositionTable &tt,
        const Zobrist &zob
    ) {
        if (depth <= 0) return -QuickSearch::search(board, alpha, beta, ply);
        const auto us = board->side;
        const int alpha0 = alpha;

        // === TT PROBE (przed generacją ruchów) ===
        const uint64_t key = zob.computeKey(*board);
        {
            auto hit = tt.probe(key, depth);
            if (hit.hit) {
                const int tts = Evaluation::from_tt_score(hit.score, ply);
                if (hit.flag == TTFlag::EXACT) return tts;
                if (hit.flag == TTFlag::LOWER && tts >= beta) return tts;
                if (hit.flag == TTFlag::UPPER && tts <= alpha) return tts;
            }
        }

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) return Evaluation::MATE - (ply * 10);
            return 0;
        }

        int best = Evaluation::NEG_INF;
        Move::Move bestMove = 0;
        int score = 0;

        // -------- Ścieżka sekwencyjna --------
        for (size_t i = 0; i < m.size(); ++i) {
            auto move = m[i];
            const auto nextChild = MoveExecutor::executeMove(board, move);
            if (MoveExecutor::isCheck(nextChild, us)) continue;

            score = -PvSearch::search(nextChild, -beta, -alpha, depth - 1, ply + 1, tt, zob);

            if (score > best) {
                best = score;
                bestMove = move;
            }
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) break;
            }
        }

        // TT store dla ścieżki sekwencyjnej
        auto flag = TTFlag::EXACT;
        if (best <= alpha0) flag = TTFlag::UPPER;
        else if (best >= beta) flag = TTFlag::LOWER;
        {
            const int stored = Evaluation::to_tt_score(best, ply);
            tt.store(key, static_cast<uint8_t>(depth), stored, flag, bestMove);
        }

        return alpha;
    }

private:
    static void workerConsumeSplitPoint(
        ThreadPool &pool,
        SplitPoint &sp,
        const int &ply,
        TranspositionTable &tt,
        const Zobrist &zob,
        std::mutex &doneMutex,
        std::condition_variable &doneCv
    ) {
        while (!sp.abort.load(std::memory_order_relaxed)) {
            const int i = sp.nextIdx.fetch_add(1, std::memory_order_relaxed);
            if (i >= static_cast<int>(sp.moves.size())) break;

            const Move::Move move = sp.moves[i];
            std::unique_ptr<Board> child = MoveExecutor::executeMove(sp.parent, move);
            if (MoveExecutor::isCheck(child, sp.parent->side)) continue;

            const int a = sp.alpha.load(std::memory_order_acquire);
            const int b = sp.beta;

            const int sc = -PvSearch::search(child, -b, -a, sp.depth - 1, ply, tt, zob);

            // CAS na α + best; cutoff na >= beta
            int prev = sp.alpha.load(std::memory_order_acquire);
            while (sc > prev && !sp.abort.load(std::memory_order_relaxed)) {
                if (sp.alpha.compare_exchange_weak(
                    prev, sc,
                    std::memory_order_acq_rel, // sukces
                    std::memory_order_acquire // porażka
                )) {
                    sp.bestScore.store(sc, std::memory_order_release);
                    sp.bestMove = move;
                    if (sc >= sp.beta) {
                        sp.abort.store(true, std::memory_order_release);
                    }
                    break;
                }
            }
        }

        // „ostatni gasi światło” – budzi czekającego
        if (sp.active.fetch_sub(1, std::memory_order_acq_rel) == 1) {
            std::lock_guard<std::mutex> lk(doneMutex);
            doneCv.notify_all();
        }
    }
};
