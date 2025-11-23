#pragma once
#include <memory>

#include "../Board/Board.hpp"
#include "../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "AlphaBeta/AlphaBeta.hpp"
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
    static RootResult run(const Board &board, const SearchConfig &config) {
        ThreadPool pool(config.threads);
        RootResult result{0, 0};

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);

        int alpha = Evaluation::NEG_INF;
        constexpr int beta = Evaluation::INF;

        for (const auto move: m) {
            auto child = MoveExecutor::executeMove(board, move);

            if (MoveExecutor::isCheck(child, board.side)) {
                continue;
            }

            const auto score = PvSplit::searchPvSplit(pool, config, child, alpha, beta, config.maxDepth - 1, 1);

            if (score > beta) {
                return {beta, move};
            }

            if (score > alpha) {
                alpha = score;
                result.score = score;
                result.bestMove = move;
            }
        }

        return result;
    }
};
