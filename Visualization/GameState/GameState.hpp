#pragma once
#include "../../Board/Board.hpp"

struct GameState {
    int selectedPiece = -1;
    Board board = {};

    std::unordered_map<int, std::vector<Move::Move> > moves = {};
};
