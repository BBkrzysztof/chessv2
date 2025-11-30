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
        Board &board,
        TranspositionTable &table,
        int alpha,
        const int beta,
        const int depth,
        const int ply
    ) {
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        const auto us = board.side;
        int best = Evaluation::NEG_INF;
        const int alpha0 = alpha;
        Move::Move bestMove = 0;

        if (auto pr = table.probe(board.zobrist, depth, ply, alpha, beta); pr.hit) {
            if (pr.flag == TTFlag::EXACT) return pr.score;
            if (pr.flag == TTFlag::LOWER && pr.score >= beta) return pr.score;
            if (pr.flag == TTFlag::UPPER && pr.score <= alpha) return pr.score;
        }

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return  -Evaluation::MATE - ply;
            }
            return 0;
        }

        const auto firstMove = m[0];
        {
            UndoInfo &undo = undoStack[ply];
            MoveExecutor::makeMove(board, firstMove, undo);

            if (!MoveExecutor::isCheck(board, us)) {
                const auto score = -searchPvSplit(pool, config, board, table, -beta, -alpha, depth - 1, ply + 1);
                MoveExecutor::unmakeMove(board, firstMove, undo);

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
            } else {
                MoveExecutor::unmakeMove(board, firstMove, undo);
            }
        }


        const auto canSplit = depth <= config.splitMinDepth;
        bool foundLegalMoves = false;

        if (!canSplit) {
            for (int i = 1; i < m.size(); i++) {
                const auto move = m[i];
                UndoInfo &undo = undoStack[ply];

                MoveExecutor::makeMove(board, move, undo);
                if (MoveExecutor::isCheck(board, us)) {
                    MoveExecutor::unmakeMove(board, move, undo);
                    continue;
                }
                foundLegalMoves = true;
                const auto sc = -AlphaBeta::search(board, table, depth - 1, -beta, -alpha, ply + 1);
                MoveExecutor::unmakeMove(board, move, undo);

                if (sc > best) {
                    best = sc;
                    bestMove = move;
                }
                if (sc > alpha) {
                    alpha = sc;
                    if (alpha >= beta) {
                        table.store(board.zobrist, depth, beta, TTFlag::LOWER, bestMove, ply);
                        return alpha;
                    }
                }
            }

            if (!foundLegalMoves) {
                if (MoveExecutor::isCheck(board, us)) {
                    return -Evaluation::MATE - ply;
                }
                return 0;
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
        const auto us = sp.parent.side;
        Board child = sp.parent;

        while (!sp.abort.load(std::memory_order_relaxed)) {
            const int i = sp.nextIdx.fetch_add(1, std::memory_order_relaxed);
            if (i >= static_cast<int>(sp.moves.size())) break;

            const Move::Move move = sp.moves[i];

            UndoInfo undo{};

            MoveExecutor::makeMove(child, move, undo);
            if (MoveExecutor::isCheck(child, us)) {
                MoveExecutor::unmakeMove(child, move, undo);
                continue;
            };

            const int a = sp.alpha.load(std::memory_order_acquire);
            const int b = sp.beta;

            const int sc = -AlphaBeta::search(child, table, sp.depth - 1, -b, -a, ply + 1);
            MoveExecutor::unmakeMove(child, move, undo);

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
