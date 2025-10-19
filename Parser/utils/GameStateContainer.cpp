#pragma once

#include <map>
#include <memory>
#include <bitset>
#include "../../Board/GameState.cpp"

using namespace std;

class GameStateContainer {

    unique_ptr<GameState> gameState;

    inline static const std::map<unsigned char, bool GameState::*> castles = {
            {'Q', &GameState::whiteQueenCastle},
            {'K', &GameState::whiteKingCastle},
            {'q', &GameState::blackQueenCastle},
            {'k', &GameState::blackKingCastle},
    };

    inline static const std::map<unsigned char, BitBoard Board::*> boards = {
            {'P', &Board::whitePawns},
            {'p', &Board::blackPawns},
            {'R', &Board::whiteRooks},
            {'r', &Board::blackRooks},
            {'N', &Board::whiteKnights},
            {'n', &Board::blackKnights},
            {'B', &Board::whiteBishops},
            {'b', &Board::blackBishops},
            {'Q', &Board::whiteQueens},
            {'q', &Board::blackQueens},
            {'K', &Board::whiteKings},
            {'k', &Board::blackKings},
    };

public:

    GameStateContainer() {
        this->gameState = std::make_unique<GameState>();
    }

    explicit GameStateContainer(unique_ptr<GameState> gameState) {
        this->gameState = std::move(gameState);
    }

    void updateCastles(const unsigned char& token, const bool value = true) {
        auto field = GameStateContainer::castles.find(token);

        *this->gameState.*(field->second) = value;
    }

    bool getWhiteQueenCastle() const {
        return this->gameState->whiteQueenCastle;
    }

    bool getBlackQueenCastle() const {
        return this->gameState->blackQueenCastle;
    }

    bool getWhiteKingCastle() const {
        return this->gameState->whiteKingCastle;
    }

    bool getBlackKingCastle() const {
        return this->gameState->blackKingCastle;
    }

    void setWhiteTurn(bool whiteTurn) {
        this->gameState->setWhiteTurn(whiteTurn);
    }

    bool isWhiteTurn() const {
        return this->gameState->isWhiteTurn();
    }

    void setEnPassantMove(const string& _enPassant) {
        this->gameState->setEnPassantMove(_enPassant);
    }

    string getEnPassantMove() const {
        return this->gameState->getEnPassantMove();
    }

    void setHalfMoveCounter(int _halfMoveCounter) {
        this->gameState->setMoveCounter(_halfMoveCounter);
    }

    int getHalfMoveCounter() const {
        return this->gameState->getHalfMoveCounter();
    }

    void setMoveCounter(int _moveCounter) {
        this->gameState->setMoveCounter(_moveCounter);
    }

    int getMoveCounter() const {
        return this->gameState->getMoveCounter();
    }

    void placePiece(const unsigned char& token, const int& row, const int& column) {
        auto field = GameStateContainer::boards.find(token);
        BitBoard temp = 1ULL << 63;

        this->gameState->board.*(field->second) |= temp >> (row * 8 + column);
    }

    map<unsigned char, bitset<64>> getBoards() const {
        map<unsigned char, bitset<64>>
                parsedBoards = {};

        for (const auto& element: GameStateContainer::boards) {
            bitset<64> bs(this->gameState->board.*(element.second));
            parsedBoards[element.first] = bs;
        }

        return parsedBoards;
    }

    unique_ptr<GameState> getGameState() {
        return std::move(this->gameState);
    }
};