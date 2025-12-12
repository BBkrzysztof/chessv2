#pragma once
#include "../../Board/Board.hpp"
#include "../MoveIndicator/MoveIndicator.hpp"

struct GameState {
    int selectedPiece = -1;
    Board board = {};
    std::optional<Move::Move> moveOrder = std::nullopt;
    std::vector<std::vector<std::pair<Move::Move, MoveIndicator> > > moves = {};

    GameState(
        const int selectedPiece,
        const Board &board
    ) : selectedPiece(selectedPiece), board(board), moves({}) {
        moves.resize(64);
    }
};
