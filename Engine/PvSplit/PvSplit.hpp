#pragma once

#include "../PvSearch/PvSearch.hpp"
#include "../ThreadPool/ThreadPool.hpp"
#include "../Utils/SplitPoint.hpp"
#include "../QuickSearch/QuickSearch.hpp"
#include "../Utils/SearchConfig.hpp"


class PvSplit {
public:
    static int searchPvSplit(
        ThreadPool &pool,
        const SearchConfig &config,
        const std::unique_ptr<Board> &board,
        int alpha,
        const int beta,
        const int depth,
        const int ply
    ) {
        if (depth <= 0) return QuickSearch::search(board, alpha, beta);
        const auto us = board->side;

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return Evaluation::MATE + ply;
            }
        }

        int best = Evaluation::MATE;
        Move::Move bestMove = 0;
        int score = 0;

        const auto firstMove = m[0];
        const auto child = MoveExecutor::executeMove(board, firstMove);

        if (!MoveExecutor::isCheck(child, us)) {
            score = -PvSearch::search(child, -beta, -alpha, depth - 1);

            if (score > best) {
                best = score;
                bestMove = firstMove;
            }

            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    return alpha;
                }
            }
        }


        const bool canSplit = depth >= config.splitMinDepth && m.size() > static_cast<size_t>(config.splitMinMoves) &&
                              pool.size() > 1;

        if (canSplit) {
            SplitPoint sp(board, alpha, beta, depth, ply, true, m);
            sp.nextIdx.store(1, std::memory_order_relaxed);
            sp.bestMove = bestMove;

            const unsigned workers = pool.size() - 1;
            for (unsigned i = 0; i < workers; ++i) {
                (void) pool.submit([&pool,&sp,us] {
                    workerConsumeSplitPoint(pool, sp, us);
                });
            }

            workerConsumeSplitPoint(pool, sp, us);

            while (sp.active.load(std::memory_order_relaxed) > 0) {
                if (!pool.helpOneRound()) {
                    std::this_thread::yield();
                }
            }
            return sp.active.load(std::memory_order_relaxed);
        }

        for (size_t i = 1; i < m.size(); ++i) {
            auto move = m[i];
            const auto nextChild = MoveExecutor::executeMove(board, move);
            if (!MoveExecutor::isCheck(nextChild, us)) {
                continue;
            }
            score = -PvSearch::search(nextChild, -(-alpha + 1), -alpha, depth - 1);

            if (score > alpha && score < beta) {
                score = -PvSearch::search(nextChild, -beta, -alpha, depth - 1);
            }
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

        return alpha;
    }

private:
    static void workerConsumeSplitPoint(ThreadPool &pool, SplitPoint &splitPoint, const PieceColor &us) {
        splitPoint.active.fetch_add(1, std::memory_order_relaxed);

        while (!splitPoint.abort.load(std::memory_order_relaxed)) {
            const int i = splitPoint.nextIdx.fetch_add(1, std::memory_order_relaxed);
            if (i >= static_cast<int>(splitPoint.moves.size())) {
                break;
            }

            const Move::Move move = splitPoint.moves[i];
            std::unique_ptr<Board> child = MoveExecutor::executeMove(splitPoint.parent, move);
            MoveExecutor::isCheck(child, splitPoint.parent->side);

            if (child->isCheck) {
                continue;
            }

            const int alpha = splitPoint.alpha.load(std::memory_order_relaxed);
            const int beta = splitPoint.beta;

            int sc = PvSearch::search(child, -(alpha + 1), -alpha, splitPoint.depth + 1);

            if (sc > alpha && sc < beta) {
                sc = PvSearch::search(child, -beta, -alpha, splitPoint.depth + 1);
                pool.helpOneRound();
            }

            int prevAlpha = splitPoint.alpha.load(std::memory_order_relaxed);
            while (sc > prevAlpha && !splitPoint.abort.load(std::memory_order_relaxed)) {
                if (splitPoint.alpha.compare_exchange_weak(prevAlpha, sc, std::memory_order_relaxed)) {
                    splitPoint.bestScore = sc;
                    splitPoint.bestMove = move;
                    if (sc > splitPoint.beta) {
                        splitPoint.abort.store(true, std::memory_order_relaxed);
                        break;
                    }
                }
            }
        }

        splitPoint.active.fetch_sub(1, std::memory_order_relaxed);
    }
};
