#pragma once
#include <memory>

#include "../../Board/Board.hpp"
#include "../QuickSearch/QuickSearch.hpp"

class PvSearch {
public:
    static int search(const std::unique_ptr<Board> &board, int alpha, const int &beta, const int &depth,
                      const int &ply) {
        if (depth <= 0) return QuickSearch::search(board, alpha, beta);
        const auto us = board->side;

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return Evaluation::MATE + ply;
            }
            return 0;
        }

        int best = Evaluation::NEG_INF;
        bool foundLegal = false;

        if (!m.empty()) {
            const auto firstMove = m[0];
            const auto child = MoveExecutor::executeMove(board, firstMove);
            if (!MoveExecutor::isCheck(child, us)) {
                const int score = -search(child, -beta, -alpha, depth - 1, ply + 1);
                foundLegal = true;

                if (score > best) {
                    best = score;
                }

                if (score > alpha) {
                    alpha = score;
                    if (alpha >= beta) return alpha;
                }
            }
        }

        for (size_t i = 1; i < m.size(); i++) {
            const auto child = MoveExecutor::executeMove(board, m[i]);
            if (MoveExecutor::isCheck(child, us)) {
                continue;
            }

            foundLegal = true;
            int score = -search(child, -(alpha + 1), -alpha, depth - 1, ply + 1);
            if (score > alpha && score < beta) {
                score = -search(child, -beta, -alpha, depth - 1, ply + 1);
            }

            if (score > best) {
                best = score;
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
                return Evaluation::MATE + ply;
            }
            return 0;
        }

        return alpha;
    }
};
