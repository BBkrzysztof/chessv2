#pragma once
#include "../../Board/Board.hpp"

class Evaluation {
public:
    static constexpr int MATE = 320000;
    static constexpr int INF = 1000000000;
    static constexpr int NEG_INF = -INF;

    static constexpr int VALUE_PAWN = 100, VALUE_KNIGHT = 320, VALUE_BISHOP = 330, VALUE_ROOK = 500, VALUE_QUEEN = 900,
            VALUE_KING = 20000;

    static constexpr short PST_PAWN[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr short PST_KNIGHT[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50
    };

    static constexpr short PST_BISHOP[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -20, -10, -10, -10, -10, -10, -10, -20
    };

    static constexpr short PST_ROOK[64] = {
        0, 0, 0, 5, 5, 0, 0, 0,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        5, 10, 10, 10, 10, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    };

    static constexpr short PST_QUEEN[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 5, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    };

    static constexpr short PST_KING[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
    };

    static int evaluate(const Board &board) {
        return getMaterialScore(board) + getPieceSquareTableScore(board);
    }

private:
    static int getPieceSquareValue(const short *pst, const uint8_t position, const PieceColor c) {
        return (c == PieceColor::WHITE) ? pst[position ^ 56] : pst[position];
    }


    static int getMaterialScore(const Board &board) {
        int score = 0;

        auto add = [&](const PieceColor &color, const PieceType &type, const int &value) {
            score += (color == PieceColor::WHITE ? +value : -value) * Bitboards::popCount64(board.pieces[color][type]);
        };

        add(PieceColor::WHITE, PieceType::PAWN, VALUE_PAWN);
        add(PieceColor::BLACK, PieceType::PAWN, VALUE_PAWN);

        add(PieceColor::WHITE, PieceType::KNIGHT, VALUE_KNIGHT);
        add(PieceColor::BLACK, PieceType::KNIGHT, VALUE_KNIGHT);

        add(PieceColor::WHITE, PieceType::BISHOP, VALUE_BISHOP);
        add(PieceColor::BLACK, PieceType::BISHOP, VALUE_BISHOP);

        add(PieceColor::WHITE, PieceType::ROOK, VALUE_ROOK);
        add(PieceColor::BLACK, PieceType::ROOK, VALUE_ROOK);

        add(PieceColor::WHITE, PieceType::QUEEN, VALUE_QUEEN);
        add(PieceColor::BLACK, PieceType::QUEEN, VALUE_QUEEN);

        return score;
    }


    static int getPieceSquareTableScore(const Board &board) {
        int score = 0;

        auto acc = [&](const PieceColor &color, const PieceType &type, const short *pst) {
            BitBoard bb = board.pieces[color][type];
            while (bb) {
                const uint8_t position = __builtin_ctzll(bb);
                score += (color == PieceColor::WHITE
                              ? getPieceSquareValue(pst, position, color)
                              : -getPieceSquareValue(pst, position, color));
                bb &=(bb - 1);
            }
        };

        acc(PieceColor::WHITE, PieceType::PAWN, PST_PAWN);
        acc(PieceColor::BLACK, PieceType::PAWN, PST_PAWN);

        acc(PieceColor::WHITE, PieceType::KNIGHT, PST_KNIGHT);
        acc(PieceColor::BLACK, PieceType::KNIGHT, PST_KNIGHT);
        //
        acc(PieceColor::WHITE, PieceType::BISHOP, PST_BISHOP);
        acc(PieceColor::BLACK, PieceType::BISHOP, PST_BISHOP);
        //
        acc(PieceColor::WHITE, PieceType::ROOK, PST_ROOK);
        acc(PieceColor::BLACK, PieceType::ROOK, PST_ROOK);
        //
        acc(PieceColor::WHITE, PieceType::QUEEN, PST_QUEEN);
        acc(PieceColor::BLACK, PieceType::QUEEN, PST_QUEEN);

        //
        acc(PieceColor::WHITE, PieceType::KING, PST_KING);
        acc(PieceColor::BLACK, PieceType::KING, PST_KING);

        return score;
    }
};
