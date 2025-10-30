#pragma once

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
    int8_t kingSq[2]{60, 4};

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
            this->kingSq[color] |= position;
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

    std::unique_ptr<Board> makeMove(const Move::Move& move) {
        const auto us = this->side;
        const auto opponent = opponentColor(us);

        const auto moveFrom = Move::moveFrom(move);
        const auto moveTo = Move::moveTo(move);
        const auto moveType = Move::moveType(move);

        const auto movedPieceCode = this->pieceOn[moveFrom];
        const auto movedPieceType = static_cast<PieceType>(movedPieceCode % 6);

        auto newBoard = std::make_unique<Board>(*this);
        newBoard->ep = -1;

        if (moveType != Move::MT_ENPASSANT) {
            const auto pieceOnTargetField = this->pieceOn[moveTo];
            if (pieceOnTargetField > 0) {
                const auto pieceOnTargetFieldType = static_cast<PieceType>(movedPieceCode % 6);

                newBoard->removePiece(opponent, pieceOnTargetFieldType, moveTo);
                newBoard->clearCastleByCapture(opponent, moveTo);
                newBoard->halfMove = 0;
            }
        }

        newBoard->clearCastleByMove(us, movedPieceType, moveFrom);

        switch (moveType) {
            case Move::MT_NORMAL:
                newBoard->movePiece(us, movedPieceType, moveFrom, moveTo);
                if (movedPieceType == PAWN) {
                    newBoard->halfMove = 0;
                    const int movedDistance = static_cast<int>(moveTo) - static_cast<int>(moveFrom);
                    if (movedDistance == 16 || movedDistance == -16) {
                        newBoard->ep = (static_cast<int>(moveTo) + static_cast<int>(moveFrom)) / 2;
                    }
                } else {
                    newBoard->halfMove++;
                }
                break;
            case Move::MT_PROMOTION:
                newBoard->removePiece(us, movedPieceType, moveFrom);
                newBoard->setPiece(us, decodePromo(Move::movePromo(move)), moveTo);
                newBoard->halfMove = 0;
                break;
            case Move::MT_CASTLE:
                if (us == WHITE) {
                    if (moveTo == 6) {
                        // short: e1->g1, h1->f1
                        newBoard->movePiece(WHITE, KING, 4, 6);
                        newBoard->movePiece(WHITE, ROOK, 7, 5);
                    } else {
                        // long: e1->c1, a1->d1
                        newBoard->movePiece(WHITE, KING, 4, 2);
                        newBoard->movePiece(WHITE, ROOK, 0, 3);
                    }
                    newBoard->castle &= ~(1 | 2);
                } else {
                    if (moveTo == 62) {
                        // short: e8->g8, h8->f8
                        newBoard->movePiece(WHITE, KING, 60, 62);
                        newBoard->movePiece(WHITE, ROOK, 63, 61);
                    } else {
                        // long: e8->c8, a8->d8
                        newBoard->movePiece(WHITE, KING, 60, 58);
                        newBoard->movePiece(WHITE, ROOK, 56, 59);
                    }
                    newBoard->castle &= ~(4 | 8);
                }
                newBoard->halfMove++;
                break;
            case Move::MT_ENPASSANT:
                const auto capturedField = us == WHITE ? static_cast<uint8_t>(moveTo - 8) : static_cast<uint8_t>(moveTo + 8);
                newBoard->removePiece(opponent, PAWN, capturedField);
                newBoard->movePiece(us, PAWN, moveFrom, moveTo);
                newBoard->halfMove = 0;
                break;
        };

        return newBoard;
    }
};
