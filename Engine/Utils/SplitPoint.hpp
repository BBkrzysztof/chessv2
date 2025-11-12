#pragma once

#include <atomic>

#include "../../Board/Board.hpp"
#include "../../MoveGenerator/Move/Move.hpp"

struct SplitPoint {
    std::atomic<int> alpha;
    const int beta;

    std::atomic<int> bestScore;
    Move::Move bestMove;

    const int depth;
    const int ply;
    const bool pvNode;

    const std::unique_ptr<Board> &parent;

    std::vector<Move::Move> moves;
    std::atomic<int> nextIdx{0};
    std::atomic<int> active{0};
    std::atomic<bool> abort{false};

    SplitPoint(const std::unique_ptr<Board> &parent, const int alpha, const int beta, const int depth, const int ply,
               const bool pvNode, const std::vector<Move::Move> &moves)
        : alpha(alpha), beta(beta), bestScore(Evaluation::NEG_INF), bestMove(0),
          depth(depth), ply(ply), pvNode(pvNode), parent(parent), moves(moves) {
        std::cout << "SplitPoint::SplitPoint()" << std::endl;
    }
};
