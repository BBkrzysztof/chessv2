#pragma once

#include "../../Board/Board.hpp"
#include "../Evaluation/Evaluation.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "../TranspositionTable/TranspositionTable.hpp"

class AlphaBeta {
public:
    static int search(
        const Board &board,
        TranspositionTable &table,
        const int depth,
        int alpha,
        const int beta,
        const int ply
    ) {
        const auto alpha0 = alpha;
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }
        table.newSearch();

        if (const auto pr = table.probe(board.zobrist, depth, ply, alpha, beta); pr.hit) {
            if (pr.flag == TTFlag::EXACT) return pr.score;
            if (pr.flag == TTFlag::LOWER && pr.score >= beta) return pr.score;
            if (pr.flag == TTFlag::UPPER && pr.score <= alpha) return pr.score;
        }
        Move::Move bestMove = 0;
        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(
            board
        );

        for (const auto &move: m) {
            auto newPosition = MoveExecutor::executeMove(board, move);

            if (MoveExecutor::isCheck(newPosition, board.side)) {
                continue;
            }

            const auto score = -search(newPosition, table, depth - 1, -beta, -alpha, ply);

            if (score >= beta) {
                table.store(board.zobrist, depth, beta, TTFlag::LOWER, move, ply);
                return beta;
            }
            if (score > alpha) {
                alpha = score;
                bestMove = move;
                if (alpha >= beta) {
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
};
