#pragma once

#include <memory>

#include "UndoInfo.hpp"
#include "../../Board/Board.hpp"
#include "../PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"

class MoveExecutor {
public:
    static Board executeMoveCopyMake(
        const Board &board,
        const Move::Move &move
    ) {
        const auto us = board.side;
        const auto opponent = opponentColor(us);

        const auto moveFrom = Move::moveFrom(move);
        const auto moveTo = Move::moveTo(move);
        const auto moveType = Move::moveType(move);

        const auto movedPieceCode = board.pieceOn[moveFrom];
        const auto movedPieceType = static_cast<PieceType>(movedPieceCode % 6);

        Board newBoard(board);
        newBoard.ep = -1;

        if (moveType != Move::MT_ENPASSANT) {
            const auto pieceOnTargetField = board.pieceOn[moveTo];
            if (pieceOnTargetField >= 0) {
                const auto pieceOnTargetFieldType = static_cast<PieceType>(pieceOnTargetField % 6);
                if (pieceOnTargetField == 25) {
                    std::cout << pieceOnTargetField << std::endl;
                }
                newBoard.removePiece(opponent, pieceOnTargetFieldType, moveTo);
                newBoard.clearCastleByCapture(opponent, moveTo);
                newBoard.halfMove = 0;
            }
        }

        newBoard.clearCastleByMove(us, movedPieceType, moveFrom);

        switch (moveType) {
            case Move::MT_NORMAL:
                newBoard.movePiece(us, movedPieceType, moveFrom, moveTo);
                if (movedPieceType == PAWN) {
                    newBoard.halfMove = 0;
                    const int movedDistance = static_cast<int>(moveTo) - static_cast<int>(moveFrom);
                    if (movedDistance == 16 || movedDistance == -16) {
                        newBoard.ep = (static_cast<int>(moveTo) + static_cast<int>(moveFrom)) / 2;
                    }
                } else {
                    newBoard.halfMove++;
                }
                break;
            case Move::MT_PROMOTION:
                newBoard.removePiece(us, movedPieceType, moveFrom);
                newBoard.setPiece(us, decodePromo(Move::movePromo(move)), moveTo);
                newBoard.halfMove = 0;
                break;
            case Move::MT_CASTLE:
                if (us == WHITE) {
                    if (moveTo == 6) {
                        // short: e1->g1, h1->f1
                        newBoard.movePiece(WHITE, KING, 4, 6);
                        newBoard.movePiece(WHITE, ROOK, 7, 5);
                    } else {
                        // long: e1->c1, a1->d1
                        newBoard.movePiece(WHITE, KING, 4, 2);
                        newBoard.movePiece(WHITE, ROOK, 0, 3);
                    }
                    newBoard.castle &= ~(1 | 2);
                } else {
                    if (moveTo == 62) {
                        // short: e8->g8, h8->f8
                        newBoard.movePiece(BLACK, KING, 60, 62);
                        newBoard.movePiece(BLACK, ROOK, 63, 61);
                    } else {
                        // long: e8->c8, a8->d8
                        newBoard.movePiece(BLACK, KING, 60, 58);
                        newBoard.movePiece(BLACK, ROOK, 56, 59);
                    }
                    newBoard.castle &= ~(4 | 8);
                }
                newBoard.halfMove++;
                break;
            case Move::MT_ENPASSANT:
                const auto capturedField = us == WHITE
                                               ? static_cast<uint8_t>(moveTo - 8)
                                               : static_cast<uint8_t>(moveTo + 8);
                newBoard.removePiece(opponent, PAWN, capturedField);
                newBoard.movePiece(us, PAWN, moveFrom, moveTo);
                newBoard.halfMove = 0;
                break;
        };

        newBoard.side = opponentColor(us);

        return newBoard;
    }

    static void makeMove(
        Board &board,
        const Move::Move &move,
        UndoInfo &info
    ) {
        info.zobristBefore = board.zobrist;
        info.castleBefore = board.castle;
        info.epBefore = board.ep;
        info.halfMoveBefore = board.halfMove;
        info.fullMoveBefore = board.fullMove;
        info.captured = board.pieceOn[Move::moveTo(move)];

        const auto us = board.side;
        const auto opponent = opponentColor(us);

        const auto moveFrom = Move::moveFrom(move);
        const auto moveTo = Move::moveTo(move);
        const auto moveType = Move::moveType(move);

        const auto movedPieceCode = board.pieceOn[moveFrom];
        const auto movedPieceType = static_cast<PieceType>(movedPieceCode % 6);

        board.ep = -1;

        if (moveType != Move::MT_ENPASSANT) {
            const auto pieceOnTargetField = board.pieceOn[moveTo];
            if (pieceOnTargetField >= 0) {
                const auto pieceOnTargetFieldType = static_cast<PieceType>(pieceOnTargetField % 6);
                if (pieceOnTargetField == 25) {
                    std::cout << pieceOnTargetField << std::endl;
                }
                board.removePiece(opponent, pieceOnTargetFieldType, moveTo);
                board.clearCastleByCapture(opponent, moveTo);
                board.halfMove = 0;
            }
        }

        board.clearCastleByMove(us, movedPieceType, moveFrom);

        switch (moveType) {
            case Move::MT_NORMAL:
                board.movePiece(us, movedPieceType, moveFrom, moveTo);
                if (movedPieceType == PAWN) {
                    board.halfMove = 0;
                    const int movedDistance = static_cast<int>(moveTo) - static_cast<int>(moveFrom);
                    if (movedDistance == 16 || movedDistance == -16) {
                        board.ep = (static_cast<int>(moveTo) + static_cast<int>(moveFrom)) / 2;
                    }
                } else {
                    board.halfMove++;
                }
                break;
            case Move::MT_PROMOTION:
                board.removePiece(us, movedPieceType, moveFrom);
                board.setPiece(us, decodePromo(Move::movePromo(move)), moveTo);
                board.halfMove = 0;
                break;
            case Move::MT_CASTLE:
                if (us == WHITE) {
                    if (moveTo == 6) {
                        // short: e1->g1, h1->f1
                        board.movePiece(WHITE, KING, 4, 6);
                        board.movePiece(WHITE, ROOK, 7, 5);
                    } else {
                        // long: e1->c1, a1->d1
                        board.movePiece(WHITE, KING, 4, 2);
                        board.movePiece(WHITE, ROOK, 0, 3);
                    }
                    board.castle &= ~(1 | 2);
                } else {
                    if (moveTo == 62) {
                        // short: e8->g8, h8->f8
                        board.movePiece(BLACK, KING, 60, 62);
                        board.movePiece(BLACK, ROOK, 63, 61);
                    } else {
                        // long: e8->c8, a8->d8
                        board.movePiece(BLACK, KING, 60, 58);
                        board.movePiece(BLACK, ROOK, 56, 59);
                    }
                    board.castle &= ~(4 | 8);
                }
                board.halfMove++;
                break;
            case Move::MT_ENPASSANT:
                const auto capturedField = us == WHITE
                                               ? static_cast<uint8_t>(moveTo - 8)
                                               : static_cast<uint8_t>(moveTo + 8);
                board.removePiece(opponent, PAWN, capturedField);
                board.movePiece(us, PAWN, moveFrom, moveTo);
                board.halfMove = 0;
                break;
        };

        board.side = opponentColor(us);
    }

    static void unmakeMove(
        Board &board,
        const Move::Move &move,
        const UndoInfo &info
    ) {
        const auto opponent = board.side;
        const auto us = opponentColor(opponent);

        const auto moveFrom = Move::moveFrom(move);
        const auto moveTo = Move::moveTo(move);
        const auto moveType = Move::moveType(move);

        switch (moveType) {
            case Move::MT_NORMAL: {
                const auto movedPieceCode = board.pieceOn[moveTo];
                const auto movedPieceType = static_cast<PieceType>(movedPieceCode % 6);

                board.movePiece(us, movedPieceType, moveTo, moveFrom);

                if (info.captured >= 0) {
                    const auto capturedType =
                            static_cast<PieceType>(info.captured % 6);
                    board.setPiece(opponent, capturedType, moveTo);
                }
                break;
            }

            case Move::MT_PROMOTION: {
                const auto promoType = decodePromo(Move::movePromo(move));

                board.removePiece(us, promoType, moveTo);
                board.setPiece(us, PAWN, moveFrom);

                if (info.captured >= 0) {
                    const auto capturedType =
                            static_cast<PieceType>(info.captured % 6);
                    board.setPiece(opponent, capturedType, moveTo);
                }
                break;
            }

            case Move::MT_CASTLE: {
                if (us == WHITE) {
                    if (moveTo == 6) {
                        board.movePiece(WHITE, KING, 6, 4);
                        board.movePiece(WHITE, ROOK, 5, 7);
                    } else {
                        board.movePiece(WHITE, KING, 2, 4);
                        board.movePiece(WHITE, ROOK, 3, 0);
                    }
                } else {
                    if (moveTo == 62) {
                        board.movePiece(BLACK, KING, 62, 60);
                        board.movePiece(BLACK, ROOK, 61, 63);
                    } else {
                        board.movePiece(BLACK, KING, 58, 60);
                        board.movePiece(BLACK, ROOK, 59, 56);
                    }
                }
                break;
            }

            case Move::MT_ENPASSANT: {
                const auto capturedField = us == WHITE
                                               ? static_cast<uint8_t>(moveTo - 8)
                                               : static_cast<uint8_t>(moveTo + 8);

                board.movePiece(us, PAWN, moveTo, moveFrom);
                board.setPiece(opponent, PAWN, capturedField);
                break;
            }
        }

        board.zobrist = info.zobristBefore;
        board.castle = info.castleBefore;
        board.ep = info.epBefore;
        board.halfMove = info.halfMoveBefore;
        board.fullMove = info.fullMoveBefore;

        board.side = us;
    }

    static bool isCheck(const Board &board, const PieceColor &us) {
        const auto enemyColor = opponentColor(us);
        const auto kingPosition = board.kingSq[us];

        const auto isCheck = PseudoLegalMovesGenerator::isSquareAttackedBy(
            kingPosition,
            enemyColor,
            board
        );

        return isCheck;
    }
};
