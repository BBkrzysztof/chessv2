#pragma once

#include "../../Board/Board.hpp"
#include "../Evaluation/Evaluation.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"

class AlphaBeta {
public:
    static int search(const Board& board, const int depth, int alpha, const int beta) {
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(
                board
        );

        for (const auto& move: m) {
            auto newPosition = MoveExecutor::executeMove(board, move);

            if (MoveExecutor::isCheck(newPosition, board.side)) {
                continue;
            }

            const auto score = -search(newPosition, depth - 1, -beta, -alpha);

            if (score >= beta) {
                return beta;
            }
            if (score > alpha) {
                alpha = score;
            }
        }
        return alpha;
    }


};