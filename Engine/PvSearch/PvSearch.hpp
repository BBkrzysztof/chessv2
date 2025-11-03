#pragma once
#include <memory>

#include "../../Board/Board.hpp"
#include "../QuickSearch/QuickSearch.hpp"

class PvSearch {
public:
    static int search(const std::unique_ptr<Board> &board, int alpha, const int &beta, const int &depth) {
        if (depth <= 0) return QuickSearch::search(board, alpha, beta);
        const auto us = board->side;

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return Evaluation::MATE;
            }
        }

        const int best = Evaluation::MATE;

        {
            const auto firstMove = m[0];
            const auto child = MoveExecutor::executeMove(board, firstMove);
            if (!MoveExecutor::isCheck(child, us)) {
                int score = -search(child, -beta, -alpha, depth - 1);
                if (best > alpha) alpha = best;
                if (alpha >= beta) return alpha;
            }
        }

        for (size_t i = 1; i < m.size(); i++) {
            const auto child = MoveExecutor::executeMove(board, m[i]);
            if (MoveExecutor::isCheck(child, us)) {
                continue;
            }

            int score = -search(child, -(alpha + 1), -alpha, depth - 1);
            if (score > alpha && score < beta) {
                score = -search(child, -beta, -alpha, depth - 1);
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
};
