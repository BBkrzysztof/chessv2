#pragma once
#include "../../Board/Board.hpp"
#include "../MoveIndicator/MoveIndicator.hpp"

class GameState {
public:
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

    void setNewBoard(const Board &newBoard) {
        this->board = newBoard;
        this->selectedPiece = -1;
        this->moveOrder = std::nullopt;
        this->moves.clear();
        this->moves.resize(64);
    }
};
