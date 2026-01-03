#pragma once
#include "../../Board/Board.hpp"
#include "../../Engine/Utils/SearchConfig.hpp"
#include "../MoveIndicator/MoveIndicator.hpp"

class GameState {
public:
    int selectedPiece = -1;
    Board board = {};
    std::optional<Move::Move> moveOrder = std::nullopt;
    std::optional<Move::Move> promotionMove = std::nullopt;
    std::vector<std::vector<std::pair<Move::Move, MoveIndicator> > > moves = {};

    std::optional<PieceColor> checkedColor = std::nullopt;
    std::optional<PieceColor> matColor = std::nullopt;

    std::atomic<bool> isAiPlayerRunning = false;

    bool isStalemate = false;
    bool isModalOpened = false;
    bool isPromotionModalOpen = false;

    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";;

    SearchConfig lim = {};

    GameState(
        const int selectedPiece,
        const Board &board
    ) : selectedPiece(selectedPiece), board(board), moves({}) {
        moves.resize(64);
        this->matColor = std::nullopt;
        this->isModalOpened = false;
    }

    void setNewBoard(const Board &newBoard) {
        this->board = newBoard;
        this->selectedPiece = -1;
        this->moveOrder = std::nullopt;
        this->moves.clear();
        this->moves.resize(64);

        this->checkedColor = std::nullopt;
        this->matColor = std::nullopt;
        this->isStalemate = false;
        this->isModalOpened = false;
        this->fen = Parser::toFEN(newBoard);
    }

    void resetBoard() {
        this->fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        this->setNewBoard(Parser::loadFen(this->fen));
    }
};
