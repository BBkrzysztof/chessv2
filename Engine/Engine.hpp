#pragma once
#include <memory>

#include "../Board/Board.hpp"
#include "../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "AlphaBeta/AlphaBeta.hpp"
#include "Evaluation/Evaluation.hpp"
#include "PvSplit/PvSplit.hpp"
#include "ThreadPool/ThreadPool.hpp"
#include "TranspositionTable/TranspositionTable.hpp"
#include "../Board/Zobrist.hpp"
#include "Utils/SearchConfig.hpp"

struct RootResult {
    int score;
    uint16_t bestMove;
};


class Engine {
public:
    static RootResult run(
        Board &board,
        const SearchConfig &config,
        TranspositionTable &table
    ) {
        ThreadPool pool(config.threads);
        RootResult result{0, 0};

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);

        int alpha = Evaluation::NEG_INF;
        constexpr int beta = Evaluation::INF;
        const auto us = board.side;

        for (const auto move: m) {
            UndoInfo &undo = undoStack[1];
            MoveExecutor::makeMove(board, move,undo);

            if (MoveExecutor::isCheck(board, us)) {
                MoveExecutor::unmakeMove(board, move,undo);
                continue;
            }

            const auto score = PvSplit::searchPvSplit(pool, config, board, table, alpha, beta, config.maxDepth - 1, 1);
            MoveExecutor::unmakeMove(board, move,undo);

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
