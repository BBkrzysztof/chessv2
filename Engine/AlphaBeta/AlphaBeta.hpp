#pragma once

#include "../../Board/Board.hpp"
#include "../Evaluation/Evaluation.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "../TranspositionTable/TranspositionTable.hpp"

constexpr int MAX_DEPTH = 128;


inline thread_local UndoInfo undoStack[MAX_DEPTH];

class AlphaBeta {
public:
    static int search(
        Board &board,
        TranspositionTable &table,
        const int depth,
        int alpha,
        const int beta,
        const int ply
    ) {
        const auto alpha0 = alpha;
        const auto us = board.side;

        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        if (const auto pr = table.probe(board.zobrist, depth, ply, alpha, beta); pr.hit) {
            if (pr.flag == TTFlag::EXACT) return pr.score;
            if (pr.flag == TTFlag::LOWER && pr.score >= beta) return pr.score;
            if (pr.flag == TTFlag::UPPER && pr.score <= alpha) return pr.score;
        }

        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(
            board
        );

        if (m.empty()) {
            if (MoveExecutor::isCheck(board, us)) {
                return  -Evaluation::MATE - ply;
            }
            return 0;
        }

        Move::Move bestMove = 0;

        bool foundLegalMoves = false;
        for (const auto &move: m) {
            UndoInfo &undo = undoStack[ply];
            MoveExecutor::makeMove(board, move, undo);

            if (MoveExecutor::isCheck(board, us)) {
                MoveExecutor::unmakeMove(board, move, undo);
                continue;
            }

            const auto score = -search(board, table, depth - 1, -beta, -alpha, ply + 1);
            foundLegalMoves = true;
            MoveExecutor::unmakeMove(board, move, undo);

            if (score >= beta) {
                table.store(board.zobrist, depth, beta, TTFlag::LOWER, move, ply);
                return beta;
            }
            if (score > alpha) {
                alpha = score;
                bestMove = move;
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
};
