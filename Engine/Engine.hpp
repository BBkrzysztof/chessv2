#pragma once
#include <memory>

#include "../Board/Board.hpp"
#include "../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "Evaluation/Evaluation.hpp"
#include "PvSplit/PvSplit.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include "TranspositionTable/TranspositionTable.hpp"
#include "TranspositionTable/Zobrist.hpp"
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

        int alpha = Evaluation::NEG_INF;
        int beta = Evaluation::INF;
        TranspositionTable tt{1u << 24};
        Zobrist zob{0xDEADBEEF};

        tt.newSearchIteration();


        auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            result = {alpha, 0};
            return result;
        };

        const auto us = board->side;
        Move::Move bestMove = m[0];

        for (size_t i = 0; i < m.size(); i++) {
            const auto child = MoveExecutor::executeMove(board, m[i]);
            if (MoveExecutor::isCheck(child, us)) {
                continue;
            }

            const int score = -PvSplit::searchPvSplit(
                pool,
                config,
                child,
                -beta,
                -alpha,
                config.maxDepth - 1,
                10,
                tt,
                zob
            );

            if (score > alpha) {
                alpha = score;
                bestMove = m[i];
            }

            Bitboards::print_bb(child->occupancyAll);
            std::cout << "Alpha: " << alpha << "  Score: " << score << std::endl;
        }

        result = {alpha, bestMove};
        return result;
    }
};
