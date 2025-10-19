#pragma once

#include "../Bitboard.h"

using namespace std;

class Board {
private:

    BitBoard whitePawns = 0;
    BitBoard blackPawns = 0;

    BitBoard whiteRooks = 0;
    BitBoard blackRooks = 0;

    BitBoard whiteKnights = 0;
    BitBoard blackKnights = 0;

    BitBoard whiteBishops = 0;
    BitBoard blackBishops = 0;

    BitBoard whiteQueens = 0;
    BitBoard blackQueens = 0;

    BitBoard whiteKings = 0;
    BitBoard blackKings = 0;


public:

    Board() = default;

    Board(const Board& other) {
        this->whitePawns = other.whitePawns;
        this->blackPawns = other.blackPawns;

        this->whiteRooks = other.whiteRooks;
        this->blackRooks = other.blackRooks;

        this->whiteKnights = other.whiteKnights;
        this->blackKnights = other.blackKnights;

        this->whiteBishops = other.whiteBishops;
        this->blackBishops = other.blackBishops;

        this->whiteQueens = other.whiteQueens;
        this->blackQueens = other.blackQueens;

        this->whiteKings = other.whiteKings;
        this->blackKings = other.blackKings;
    }

    friend class GameStateContainer;
};

