#pragma once

#include <atomic>

#include "../../Board/Board.hpp"
#include "../../MoveGenerator/Move/Move.hpp"

struct SplitPoint {
    std::atomic<int> alpha;
    const int beta;

    int bestScore;
    Move::Move bestMove;

    const int depth;
    const int ply;
    const bool pvNode;

    const Board parent;

    std::vector<Move::Move> moves;
    std::atomic<int> nextIdx{0};
    std::atomic<int> active{0};
    std::atomic<bool> abort{false};
};
