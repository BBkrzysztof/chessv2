#pragma once

#include <algorithm>
#include "../../Board/Board.hpp"
#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../Evaluation/Evaluation.hpp"

class QuickSearch {
public:
    static int search(const unique_ptr<Board> &board, int alpha, const int beta) {
        const auto us = board->side;

        if (MoveExecutor::isCheck(board, us)) {
            auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
            bool found = false;
            int best = Evaluation::MATE;

            for (const auto &move: m) {
                auto child = MoveExecutor::executeMove(board, move);

                if (MoveExecutor::isCheck(child, us)) {
                    continue;
                }

                found = true;

                const int score = -search(child, -beta, -alpha);
                if (score > best) { best = score; }
                if (score > alpha) {
                    alpha = score;
                    if (alpha >= beta) {
                        return alpha;
                    }
                }
            }

            if (!found) {
                return -Evaluation::MATE;
            }
            return alpha;
        }

        const int stand = Evaluation::evaluate(board);

        if (stand >= beta) { return stand; }
        if (stand > alpha) { alpha = stand; }

        Move::MoveList captures;
        captures.m.reserve(32);

        generateCaptures(board, captures);

        for (const auto &move: captures.m) {
            const auto child = MoveExecutor::executeMove(board, move);

            if (MoveExecutor::isCheck(child, us)) {
                continue;
            }

            const int score = -search(child, -beta, -alpha);
            if (score > alpha) {
                alpha = score;
                if (alpha >= beta) {
                    return alpha;
                }
            }
        }

        return alpha;
    }

private:
    static int getPieceValue(const PieceType &type) {
        switch (type) {
            case PieceType::PAWN: return Evaluation::VALUE_PAWN;
            case PieceType::KNIGHT: return Evaluation::VALUE_KNIGHT;
            case PieceType::BISHOP: return Evaluation::VALUE_BISHOP;
            case PieceType::ROOK: return Evaluation::VALUE_ROOK;
            case PieceType::QUEEN: return Evaluation::VALUE_QUEEN;
            case PieceType::KING: return Evaluation::VALUE_KING;
            default: return 0;
        }
    }

    static int mvvLvaScore(const Move::Move &move, const std::unique_ptr<Board> &board) {
        const uint8_t from = Move::moveFrom(move);
        const uint8_t to = Move::moveTo(move);
        PieceType victimType;

        if (Move::moveType(move) == Move::MT_ENPASSANT) {
            const uint8_t victimSq = (board->side == PieceColor::WHITE) ? (to - 8) : (to + 8);
            victimType = static_cast<PieceType>(board->pieceOn[victimSq] % 6);
        } else {
            victimType = static_cast<PieceType>(board->pieceOn[to] % 6);
        }

        const auto attacker = static_cast<PieceType>(board->pieceOn[from] % 6);
        return getPieceValue(victimType) * 16 - getPieceValue(attacker);
    }

    static void generateCaptures(const std::unique_ptr<Board> &board, Move::MoveList &list) {
        const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);

        const BitBoard opponentOccupation = board->occupancy[opponentColor(board->side)];

        for (auto move: m) {
            const auto type = Move::moveType(move);
            if (type == Move::MT_ENPASSANT) {
                list.push(move);
                continue;
            }
            const uint8_t to = Move::moveTo(move);

            if (Bitboards::bit(to) & opponentOccupation) {
                list.push(move);
            } else if (type == Move::MT_PROMOTION) {
                if (Bitboards::bit(to) & opponentOccupation) {
                    list.push(move);
                }
            }
        }

        std::sort(
            list.m.begin(),
            list.m.end(),
            [&](const Move::Move a, const Move::Move b) { return mvvLvaScore(a, board) > mvvLvaScore(b, board); }
        );
    }
};
