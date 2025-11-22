#pragma once
#include "../../Bitboard.h"
#include "../../Board/Board.hpp"
#include "../PreComputedMoves/PreComputedMoves.hpp"

inline const PreComputedMoves &preComputedMoves = PreComputedMovesGenerator::instance();

class PseudoLegalMovesGenerator {
public:
    static BitBoard getFieldsAttackedByColor(
        const PieceColor &color,
        const Board &board
    ) {
        const BitBoard occupancyAll = board.occupancyAll;
        BitBoard attacks = 0ULL;

        const BitBoard pawns = board.pieces[color][PieceType::PAWN];
        if (color == PieceColor::WHITE) {
            attacks |= ((pawns << 7) & ~Bitboards::FILE_H) | ((pawns << 9) & ~Bitboards::FILE_A);
        } else {
            attacks |= ((pawns >> 7) & ~Bitboards::FILE_A) | ((pawns >> 9) & ~Bitboards::FILE_H);
        }

        const BitBoard knights = board.pieces[color][PieceType::KNIGHT];
        for (BitBoard temp = knights; temp; Bitboards::pop_lsb(temp)) {
            const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            attacks |= preComputedMoves.knight[position];
        }

        const BitBoard bishops = board.pieces[color][PieceType::BISHOP];
        for (BitBoard temp = bishops; temp; Bitboards::pop_lsb(temp)) {
            const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.bishopMask[position];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            attacks |= preComputedMoves.bishop[position][idx];
        }

        const BitBoard rooks = board.pieces[color][PieceType::ROOK];
        for (BitBoard temp = rooks; temp; Bitboards::pop_lsb(temp)) {
            const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.rookMask[position];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            attacks |= preComputedMoves.rook[position][idx];
        }

        const BitBoard queens = board.pieces[color][PieceType::QUEEN];
        for (BitBoard temp = queens; temp; Bitboards::pop_lsb(temp)) {
            const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.queenMask[position];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            attacks |= preComputedMoves.queen[position][idx];
        }

        const BitBoard king = board.pieces[color][PieceType::KING];
        attacks |= preComputedMoves.king[Bitboards::lsb_index(king)];

        return attacks;
    }

    static Move::MoveList generatePseudoLegalMoves(
        const Board &board
    ) {
        Move::MoveList moves;

        const PieceColor side = board.side;

        getQueenMoves(side, board, moves);
        getRookMoves(side, board, moves);
        getBishopMoves(side, board, moves);
        getKnightMoves(side, board, moves);

        if (side == PieceColor::WHITE) {
            getWhitePawnMoves(board, moves);
        } else {
            getBlackPawnMoves(board, moves);
        }

        getKingMovesWithoutCastle(side, board, moves);

        getCastles(side, board, moves);

        return moves;
    }


    static bool isSquareAttackedBy(
        const uint8_t position,
        const PieceColor color,
        const Board &board
    ) {
        const BitBoard occ = board.occupancyAll;

        const BitBoard enemyPawns = board.pieces[color][PieceType::PAWN];
        const BitBoard enemyKnights = board.pieces[color][PieceType::KNIGHT];
        const BitBoard enemyBishops = board.pieces[color][PieceType::BISHOP];
        const BitBoard enemyRooks = board.pieces[color][PieceType::ROOK];
        const BitBoard enemyQueens = board.pieces[color][PieceType::QUEEN];
        const BitBoard enemyKing = board.pieces[color][PieceType::KING];

        const BitBoard sqBB = 1ULL << position;

        if (color == PieceColor::WHITE) {
            const BitBoard sources =
                    ((sqBB >> 7) & ~Bitboards::FILE_H) |
                    ((sqBB >> 9) & ~Bitboards::FILE_A);
            if (enemyPawns & sources) return true;
        } else {
            const BitBoard sources =
                    ((sqBB << 7) & ~Bitboards::FILE_A) |
                    ((sqBB << 9) & ~Bitboards::FILE_H);
            if (enemyPawns & sources) return true;
        }

        if (enemyKnights & preComputedMoves.knight[position]) return true;

        if (enemyKing & preComputedMoves.king[position]) return true;

        {
            const BitBoard mask = preComputedMoves.bishopMask[position];
            const BitBoard relevant = mask & occ;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);
            const BitBoard attacksFromSq = preComputedMoves.bishop[position][idx];

            if (attacksFromSq & (enemyBishops | enemyQueens)) return true;
        }

        {
            const BitBoard mask = preComputedMoves.rookMask[position];
            const BitBoard relevant = mask & occ;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);
            const BitBoard attacksFromSq = preComputedMoves.rook[position][idx];

            if (attacksFromSq & (enemyRooks | enemyQueens)) return true;
        }

        return false;
    }

private:
    static void getKingMovesWithoutCastle(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const BitBoard friendllyOccupation = board.occupancy[color];
        const auto from = static_cast<uint8_t>(Bitboards::lsb_index(board.pieces[color][PieceType::KING]));
        const auto moves = preComputedMoves.king[from] & ~friendllyOccupation;

        emitMoves(from, moves, moveList);
    }

    static void getKnightMoves(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const BitBoard friendllyOccupation = board.occupancy[color];

        for (BitBoard temp = board.pieces[color][PieceType::KNIGHT]; temp; Bitboards::pop_lsb(temp)) {
            const auto from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto moves = preComputedMoves.knight[from] & ~friendllyOccupation;
            emitMoves(from, moves, moveList);
        }
    }

    static void getBishopMoves(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const BitBoard friendllyOccupation = board.occupancy[color];
        const BitBoard occupancyAll = board.occupancyAll;

        for (BitBoard temp = board.pieces[color][PieceType::BISHOP]; temp; Bitboards::pop_lsb(temp)) {
            const auto from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.bishopMask[from];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            const auto moves = preComputedMoves.bishop[from][idx] & ~friendllyOccupation;

            emitMoves(from, moves, moveList);
        }
    }

    static void getRookMoves(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const BitBoard friendllyOccupation = board.occupancy[color];
        const BitBoard occupancyAll = board.occupancyAll;

        for (BitBoard temp = board.pieces[color][PieceType::ROOK]; temp; Bitboards::pop_lsb(temp)) {
            const auto from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.rookMask[from];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            const auto moves = preComputedMoves.rook[from][idx] & ~friendllyOccupation;

            emitMoves(from, moves, moveList);
        }
    }

    static void getQueenMoves(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const BitBoard friendllyOccupation = board.occupancy[color];
        const BitBoard occupancyAll = board.occupancyAll;

        for (BitBoard temp = board.pieces[color][PieceType::QUEEN]; temp; Bitboards::pop_lsb(temp)) {
            const auto from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const auto mask = preComputedMoves.queenMask[from];

            const auto relevant = mask & occupancyAll;
            const auto idx = MagicBoardIndexGenerator::getId(relevant, mask);

            const auto moves = preComputedMoves.queen[from][idx] & ~friendllyOccupation;

            emitMoves(from, moves, moveList);
        }
    }

    static void getWhitePawnMoves(const Board &board, Move::MoveList &moveList) {
        const BitBoard enemyOccupation = board.occupancy[PieceColor::BLACK];
        const BitBoard empty = ~board.occupancyAll;

        const BitBoard pawns = board.pieces[PieceColor::WHITE][PieceType::PAWN];

        const BitBoard pushByOne = (pawns << 8) & empty;
        const BitBoard promotion = pushByOne & Bitboards::ROW_8;
        const BitBoard normalMove = pushByOne & ~Bitboards::ROW_8;

        for (BitBoard temp = normalMove; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to - 8;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
        }

        for (BitBoard temp = promotion; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to - 8;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_QUEEN));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_ROOK));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_BISHOP));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_KNIGHT));
        }

        const BitBoard pushByTwoBetweenFields = ((pawns & Bitboards::ROW_2) << 8) & empty;
        const BitBoard pushByTwo = (pushByTwoBetweenFields << 8) & empty;

        for (BitBoard temp = pushByTwo; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to - 16;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
        }

        const BitBoard captureLeft = ((pawns << 7) & ~Bitboards::FILE_H) & enemyOccupation;
        const BitBoard captureRight = ((pawns << 9) & ~Bitboards::FILE_A) & enemyOccupation;

        auto emitCapture = [&](const BitBoard &capture, const int &shift) {
            for (BitBoard temp = capture; temp; Bitboards::pop_lsb(temp)) {
                const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                const uint8_t from = to - shift;
                if (to >= 56) {
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_QUEEN));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_ROOK));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_BISHOP));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_KNIGHT));
                } else {
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
                }
            }
        };

        emitCapture(captureLeft, 7);
        emitCapture(captureRight, 9);

        if (board.ep != -1) {
            const auto ep = static_cast<uint8_t>(board.ep);
            const auto enpassantBitboard = Bitboards::bit(ep);
            BitBoard src = ((enpassantBitboard >> 7) & ~Bitboards::FILE_A) | (
                               (enpassantBitboard >> 9) & ~Bitboards::FILE_H);
            src &= pawns;

            for (BitBoard temp = src; temp; Bitboards::pop_lsb(temp)) {
                auto const from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                moveList.push(Move::encodeMove(from, ep, Move::MoveType::MT_ENPASSANT));
            }
        }
    }

    static void getBlackPawnMoves(const Board &board, Move::MoveList &moveList) {
        const BitBoard enemyOccupation = board.occupancy[PieceColor::WHITE];
        const BitBoard empty = ~board.occupancyAll;

        const BitBoard pawns = board.pieces[PieceColor::BLACK][PieceType::PAWN];

        const BitBoard pushByOne = (pawns >> 8) & empty;
        const BitBoard promotion = pushByOne & Bitboards::ROW_1;
        const BitBoard normalMove = pushByOne & ~Bitboards::ROW_1;

        for (BitBoard temp = normalMove; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to + 8;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
        }

        for (BitBoard temp = promotion; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to + 8;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_QUEEN));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_ROOK));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_BISHOP));
            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_KNIGHT));
        }

        const BitBoard pushByTwoBetweenFields = ((pawns & Bitboards::ROW_7) >> 8) & empty;
        const BitBoard pushByTwo = (pushByTwoBetweenFields >> 8) & empty;

        for (BitBoard temp = pushByTwo; temp; Bitboards::pop_lsb(temp)) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
            const uint8_t from = to + 16;

            moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
        }

        const BitBoard captureLeft = ((pawns >> 9) & ~Bitboards::FILE_H) & enemyOccupation;
        const BitBoard captureRight = ((pawns >> 7) & ~Bitboards::FILE_A) & enemyOccupation;

        auto emitCapture = [&](const BitBoard &capture, const int &shift) {
            for (BitBoard temp = capture; temp; Bitboards::pop_lsb(temp)) {
                const auto to = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                const uint8_t from = to + shift;
                if (to <= 8) {
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_QUEEN));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_ROOK));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_BISHOP));
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, Move::Promo::PR_KNIGHT));
                } else {
                    moveList.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
                }
            }
        };

        emitCapture(captureLeft, 9);
        emitCapture(captureRight, 7);

        if (board.ep != -1) {
            const auto ep = static_cast<uint8_t>(board.ep);
            const auto enpassantBitboard = Bitboards::bit(ep);
            BitBoard src = ((enpassantBitboard >> 9) & ~Bitboards::FILE_H) | (
                               (enpassantBitboard << 7) & ~Bitboards::FILE_A);
            src &= pawns;

            for (BitBoard temp = src; temp; Bitboards::pop_lsb(temp)) {
                auto const from = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                moveList.push(Move::encodeMove(from, ep, Move::MoveType::MT_ENPASSANT));
            }
        }
    }

    static void getCastles(
        const PieceColor &color,
        const Board &board,
        Move::MoveList &moveList
    ) {
        const auto kingPosition = board.kingSq[color];
        const auto enemyColor = opponentColor(color);

        auto empty = [&](const std::initializer_list<uint8_t> &sqs) {
            for (const uint8_t &s: sqs) {
                if (board.occupancyAll & Bitboards::bit(s)) return false;
            }
            return true;
        };
        auto safe = [&](const std::initializer_list<uint8_t> &sqs) {
            for (const uint8_t &s: sqs) {
                if (isSquareAttackedBy(s, enemyColor, board)) return false;
            }
            return true;
        };

        if (color == WHITE) {
            if ((board.castle & 1) && empty({5, 6}) && safe({4, 5, 6}))
                moveList.push(Move::encodeMove(kingPosition, 6, Move::MoveType::MT_CASTLE));
            if ((board.castle & 2) && empty({3, 2, 1}) && safe({4, 3, 2}))
                moveList.push(Move::encodeMove(kingPosition, 2, Move::MoveType::MT_CASTLE));
        } else {
            if ((board.castle & 4) && empty({61, 62}) && safe({60, 61, 62}))
                moveList.push(Move::encodeMove(kingPosition, 62, Move::MoveType::MT_CASTLE));
            if ((board.castle & 8) && empty({59, 58, 57}) && safe({60, 59, 58}))
                moveList.push(Move::encodeMove(kingPosition, 58, Move::MoveType::MT_CASTLE));
        }
    }


    static void emitMoves(const uint8_t &from, BitBoard dest, Move::MoveList &out) {
        while (dest) {
            const auto to = static_cast<uint8_t>(Bitboards::lsb_index(dest));
            out.push(Move::encodeMove(from, to, Move::MoveType::MT_NORMAL));
            Bitboards::pop_lsb(dest);
        }
    }
};
