#pragma once

#include "../PvSearch/PvSearch.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include "../Utils/SplitPoint.hpp"
#include "../QuickSearch/QuickSearch.hpp"
#include "../Utils/SearchConfig.hpp"
#include "../TranspositionTable/TranspositionTable.hpp"

class PvSplit {
public:
    static int searchPvSplit(
        ThreadPool &pool,
        const SearchConfig &config,
        const Board &board,
        TranspositionTable &table,
        int alpha,
        const int beta,
        const int depth,
        const int ply
    ) {
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        int best = Evaluation::NEG_INF;
        const int alpha0 = alpha;
        Move::Move bestMove = 0;

        if (auto pr = table.probe(board.zobrist, depth, ply, alpha, beta); pr.hit) {
            if (pr.flag == TTFlag::EXACT) return pr.score;
            if (pr.flag == TTFlag::LOWER && pr.score >= beta) return pr.score;
            if (pr.flag == TTFlag::UPPER && pr.score <= alpha) return pr.score;
        }

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        const auto firstMove = m[0];

        auto firstChild = MoveExecutor::executeMove(board, firstMove);
        if (!MoveExecutor::isCheck(firstChild, board.side)) {
            const auto score = -searchPvSplit(pool, config, firstChild, table, -beta, -alpha, depth - 1, ply + 1);
            if (score > best) {
                best = score;
                bestMove = firstMove;
            }
            if (best > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    table.store(board.zobrist, depth, beta, TTFlag::LOWER, bestMove, ply);
                    return alpha;
                }
            }
        }

        const auto canSplit = ply >=  config.splitMinDepth;
        if (!canSplit) {
            for (int i = 1; i < m.size(); i++) {
                auto child = MoveExecutor::executeMove(board, m[i]);
                if (MoveExecutor::isCheck(child, board.side)) {
                    continue;
                }

                auto sc = -AlphaBeta::search(child, table, depth - 1, -beta, -alpha, ply + 1);
                if (sc > best) {
                    best = sc;
                    bestMove = m[i];
                }
                if (sc > alpha) {
                    alpha = sc;
                    if (alpha >= beta) {
                        table.store(board.zobrist, depth, beta, TTFlag::LOWER, bestMove, ply);
                        return alpha;
                    }
                }
            }

            TTFlag flag;
            if (alpha <= alpha0) {
                flag = TTFlag::UPPER;
            } else {
                flag = TTFlag::EXACT;
            }

            table.store(board.zobrist, depth, alpha, flag, bestMove, ply);
            return alpha;
        }

        SplitPoint sp{board, alpha, beta, depth, ply, true, m};
        sp.nextIdx.store(1, std::memory_order_relaxed);
        sp.bestMove = bestMove;
        sp.bestScore.store(best, std::memory_order_relaxed);


        const unsigned toSpawn = std::min<unsigned>(pool.size() - 1, (m.size() > 1 ? m.size() - 1 : 0));
        sp.active.store(static_cast<int>(toSpawn) + 1, std::memory_order_relaxed);

        std::mutex doneMutex;
        std::condition_variable doneCv;

        for (unsigned int i = 0; i < toSpawn; i++) {
            pool.submit([&table, &doneMutex, ply, &sp, &doneCv]() {
                workerConsumeSplitPoint(table, sp, ply, doneMutex, doneCv);
            });
        }

        workerConsumeSplitPoint(table, sp, ply, doneMutex, doneCv);

        {
            std::unique_lock<std::mutex> lk(doneMutex);
            doneCv.wait(lk, [&] {
                return sp.active.load(std::memory_order_acquire) == 0;
            });
        }

        const int a = sp.alpha.load(std::memory_order_relaxed);
        const int bst = sp.bestScore.load(std::memory_order_relaxed);
        const int finalBest = std::max(a, bst);

        if (finalBest >= beta) {
            table.store(board.zobrist, depth, beta, TTFlag::LOWER, sp.bestMove, ply);
            return beta;
        }

        TTFlag flag;
        if (finalBest <= alpha0) {
            flag = TTFlag::UPPER;
        } else {
            flag = TTFlag::EXACT;
        }

        table.store(board.zobrist, depth, finalBest, flag, sp.bestMove, ply);
        return finalBest;
    }

private:
    static void workerConsumeSplitPoint(
        TranspositionTable &table,
        SplitPoint &sp,
        const int ply,
        std::mutex &doneMutex,
        std::condition_variable &doneCv
    ) {
        while (!sp.abort.load(std::memory_order_relaxed)) {
            const int i = sp.nextIdx.fetch_add(1, std::memory_order_relaxed);
            if (i >= static_cast<int>(sp.moves.size())) break;

            const Move::Move move = sp.moves[i];
            const auto us = sp.parent.side;
            auto child = MoveExecutor::executeMove(sp.parent, move);
            if (MoveExecutor::isCheck(child, us)) continue;

            const int a = sp.alpha.load(std::memory_order_acquire);
            const int b = sp.beta;

            const int sc = -AlphaBeta::search(child, table, sp.depth - 1, -b, -a, ply + 1);

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
