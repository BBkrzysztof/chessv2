#include <memory>

#include "../../Board/Board.hpp"
#include "../PreComputedMoves/PreComputedMoves.hpp"
#include "../PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"

class MoveExecutor {
public:
    static std::unique_ptr<Board> executeMove(
        const std::unique_ptr<Board> &board,
        const std::unique_ptr<PreComputedMoves> &preComputedMoves,
        const Move::Move &move
    ) {
        const auto us = board->side;
        const auto opponent = opponentColor(us);

        const auto moveFrom = Move::moveFrom(move);
        const auto moveTo = Move::moveTo(move);
        const auto moveType = Move::moveType(move);

        const auto movedPieceCode = board->pieceOn[moveFrom];
        const auto movedPieceType = static_cast<PieceType>(movedPieceCode % 6);

        auto newBoard = std::make_unique<Board>(*board);
        newBoard->ep = -1;

        if (moveType != Move::MT_ENPASSANT) {
            const auto pieceOnTargetField = board->pieceOn[moveTo];
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
                const auto capturedField = us == WHITE
                                               ? static_cast<uint8_t>(moveTo - 8)
                                               : static_cast<uint8_t>(moveTo + 8);
                newBoard->removePiece(opponent, PAWN, capturedField);
                newBoard->movePiece(us, PAWN, moveFrom, moveTo);
                newBoard->halfMove = 0;
                break;
        };

        const auto enemyColor = opponentColor(us);
        const auto kingPosition = newBoard->kingSq[us];

        const auto isCheck = PseudoLegalMovesGenerator::isSquareAttackedBy(
            kingPosition,
            enemyColor,
            newBoard,
            preComputedMoves
        );

        newBoard->isCheck = isCheck;
        newBoard->side = enemyColor;

        return newBoard;
    }
};
