#pragma once
#include <memory>

#include "../Board/Board.hpp"
#include "../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "Evaluation/Evaluation.hpp"
#include "PvSplit/PvSplit.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include "Utils/SearchConfig.hpp"

struct RootResult {
    int score;
    uint16_t bestMove;
};


class Engine {
public:
    static RootResult run(const std::unique_ptr<Board> &board, const SearchConfig &config) {
        ThreadPool pool(config.threads);
        RootResult result{0, 0};

        int alpha = Evaluation::MATE;
        int beta = -Evaluation::MATE;

        for (int depth = 1; depth < config.maxDepth; ++depth) {
            int a = alpha;
            const int b = beta;

            const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
            if (m.empty()) break;

            const auto us = board->side;
            int bestScore = Evaluation::MATE;
            Move::Move bestMove = m[0];

            {
                const auto child = MoveExecutor::executeMove(board, m[0]);
                if (!MoveExecutor::isCheck(child, us)) {
                    bestScore = -PvSplit::searchPvSplit(pool, config, child, -b, -a, depth - 1, 1);
                    if (bestScore > a) {
                        a = bestScore;
                        bestMove = m[0];
                    }
                }
            }

            for (size_t i = 1; i < m.size(); ++i) {
                const auto child = MoveExecutor::executeMove(board, m[i]);
                if (MoveExecutor::isCheck(child, us)) {
                    continue;
                }

                int score = -PvSplit::searchPvSplit(pool, config, child, -(a + 1), -a, depth - 1, 1);

                if (score > a && score < b) {
                    score = -PvSplit::searchPvSplit(pool, config, child, -b, -a, depth - 1, 1);
                }

                if (score > a) {
                    a = score;
                    bestMove = m[i];
                }
            }

            result = {a, bestMove};
            alpha = a - 50;
            beta = a + 50;
        }

        return result;
    }
};
