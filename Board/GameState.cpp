#pragma once

#include "Board.cpp"

using namespace std;

class GameState {
private:
    Board board;
    bool whiteQueenCastle = false;
    bool blackQueenCastle = false;
    bool whiteKingCastle = false;
    bool blackKingCastle = false;

    bool whiteTurn = true;

    string enPassant = "-";

    int halfMoveCounter = 0;
    int moveCounter = 0;

public:

    GameState() = default;

    explicit GameState(
            Board& board,
            bool whiteQueenCastle = false,
            bool blackQueenCastle = false,
            bool whiteKingCastle = false,
            bool blackKingCastle = false
    ) {
        this->board = board;
        this->whiteQueenCastle = whiteQueenCastle;
        this->blackQueenCastle = blackQueenCastle;
        this->whiteKingCastle = whiteKingCastle;
        this->blackKingCastle = blackKingCastle;
    }

    bool getWhiteQueenCastle() const {
        return this->whiteQueenCastle;
    }

    bool getBlackQueenCastle() const {
        return this->blackQueenCastle;
    }

    bool getWhiteKingCastle() const {
        return this->whiteKingCastle;
    }

    bool getBlackKingCastle() const {
        return this->blackKingCastle;
    }

    void setWhiteTurn(bool isWhiteTurn) {
        this->whiteTurn = isWhiteTurn;
    }

    bool isWhiteTurn() const {
        return this->whiteTurn;
    }

    int getHalfMoveCounter() const {
        return this->halfMoveCounter;
    }

    void setHalfMoveCounter(int _halfMoveCounter) {
        this->halfMoveCounter = _halfMoveCounter;
    }

    int getMoveCounter() const {
        return this->moveCounter;
    }

    void setMoveCounter(int _moveCounter) {
        this->moveCounter = _moveCounter;
    }

    string getEnPassantMove() const {
        return this->enPassant;
    }

    void setEnPassantMove(const string& _enPassant) {
        this->enPassant = _enPassant;
    }

    friend class GameStateContainer;
};