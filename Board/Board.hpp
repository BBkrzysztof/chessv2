#pragma once

#include <memory>
#include <algorithm>

#include "../Bitboard.h"
#include "../MoveGenerator/Move/Move.hpp"

using namespace std;

enum PieceColor {
    WHITE = 0,
    BLACK = 1
};

enum PieceType {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    EMPTY = 6
};

inline PieceColor opponentColor(const PieceColor color) { return color == WHITE ? BLACK : WHITE; }

inline PieceType decodePromo(const Move::Promo promo) {
    switch (promo) {
        case Move::PR_KNIGHT:
            return KNIGHT;
        case Move::PR_BISHOP:
            return BISHOP;
        case Move::PR_ROOK:
            return ROOK;
        case Move::PR_QUEEN:
            return QUEEN;
    }
}

class Board {
public:
    BitBoard pieces[2][6]{};
    BitBoard occupancy[2]{};
    BitBoard occupancyAll{};
    int8_t pieceOn[64]{};
    PieceColor side = WHITE;
    int castle = 0;
    int ep = -1;
    int halfMove = 0;
    int fullMove = 1;
    uint8_t kingSq[2]{60, 4};
    bool isCheck = false;

public:
     Board() = default;

    ~Board() = default;

    Board(const Board &other) noexcept {
        std::copy_n(&other.pieces[0][0], 2 * 6, &this->pieces[0][0]);
        std::copy_n(other.occupancy, 2, this->occupancy);
        this->occupancyAll = other.occupancyAll;
        std::copy_n(other.pieceOn, 64, this->pieceOn);
        this->side = other.side;
        this->castle = other.castle;
        this->ep = other.ep;
        this->halfMove = other.halfMove;
        this->fullMove = other.fullMove;
        this->kingSq[WHITE] = other.kingSq[WHITE];
        this->kingSq[BLACK] = other.kingSq[BLACK];
    }

    void setPiece(
        const PieceColor &color,
        const PieceType &type,
        const uint8_t &position
    ) {
        const BitBoard board = Bitboards::bit(position);
        this->pieceOn[position] = static_cast<int8_t>(color * 6 + type);
        this->pieces[color][type] |= board;
        this->occupancy[color] |= board;
        this->occupancyAll |= board;


        if (type == KING) {
            this->kingSq[color] = position;
        }
    }

    void removePiece(
        const PieceColor &color,
        const PieceType &type,
        const uint8_t &position
    ) {
        const BitBoard board = Bitboards::bit(position);

        this->pieceOn[position] = -1;
        this->pieces[color][type] &= ~board;
        this->occupancy[color] &= ~board;
        this->occupancyAll &= ~board;
    }

    void movePiece(
        const PieceColor &color,
        const PieceType &type,
        const uint8_t &from,
        const uint8_t &to
    ) {
        const BitBoard boardFrom = Bitboards::bit(from);
        const BitBoard boardTo = Bitboards::bit(to);

        this->pieces[color][type] ^= (boardFrom | boardTo);
        this->occupancy[color] ^= (boardFrom | boardTo);
        this->occupancyAll ^= (boardFrom | boardTo);
        this->pieceOn[from] = -1;
        this->pieceOn[to] = static_cast<int8_t>(color * 6 + type);
    }

    void clearCastleByMove(
        const PieceColor &c,
        const PieceType &type,
        const uint8_t &from
    ) {
        if (type == KING) {
            if (c == WHITE) this->castle &= ~(1 | 2);
            else this->castle &= ~(4 | 8);
        }
        if (type == ROOK) {
            if (c == WHITE && from == 0) this->castle &= ~2; // a1
            if (c == WHITE && from == 7) this->castle &= ~1; // h1
            if (c == BLACK && from == 56) this->castle &= ~8; // a8
            if (c == BLACK && from == 63) this->castle &= ~4; // h8
        }
    }

    void clearCastleByCapture(const PieceColor &capturedColor, const uint8_t &position) {
        if (capturedColor == WHITE) {
            if (position == 0) this->castle &= ~2;
            if (position == 7) this->castle &= ~1;
        } else {
            if (position == 56) this->castle &= ~8;
            if (position == 63) this->castle &= ~4;
        }
    }
};
