#pragma once
#include "../../Board/Board.hpp"
#include "../../MoveGenerator/Move/Move.hpp"
#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../GameState/GameState.hpp"

class MovesGenerator {
public:
    static void emit(
        const PieceColor color,
        const PieceType pieceType,
        const Board &board,
        const uint8_t position,
        GameState &gameState
    ) {
        auto moveList = generateMoves(color, pieceType, board);
        moveList.m.erase(
            std::remove_if(
                moveList.m.begin(),
                moveList.m.end(),
                [&board, &position](const Move::Move &move) {
                    if (Move::moveFrom(move) != position) return true;

                    const auto child = MoveExecutor::executeMoveCopyMake(board, move);
                    return MoveExecutor::isCheck(child, board.side);
                }
            ),
            moveList.m.end()
        );

        std::vector<std::pair<Move::Move, MoveIndicator> > indicators;
        indicators.reserve(moveList.m.size());

        for (const auto &move: moveList.m) {
            auto type = static_cast<VisualizationMoveType>(Move::moveType(move));
            if (Bitboards::bit(Move::moveTo(move)) & board.occupancy[opponentColor(board.side)]) {
                type = MT_CAPTURE;
            }

            std::pair<Move::Move, MoveIndicator> moveIndicatorPair = {
                move,
                {
                    color,
                    pieceType,
                    static_cast<uint8_t>(Move::moveTo(move) ^ 56),
                    type
                }
            };

            indicators.push_back(moveIndicatorPair);
        }

        gameState.moves[position] = indicators;
    }

private:
    static Move::MoveList generateMoves(const PieceColor color, const PieceType pieceType, const Board &board) {
        Move::MoveList moves;
        moves.m.reserve(128);


        switch (pieceType) {
            case PieceType::PAWN: {
                if (color == PieceColor::WHITE) {
                    PseudoLegalMovesGenerator::getWhitePawnMoves(board, moves);
                    return moves;
                }
                PseudoLegalMovesGenerator::getBlackPawnMoves(board, moves);
                return moves;
            }
            case PieceType::KNIGHT:
                PseudoLegalMovesGenerator::getKnightMoves(color, board, moves);
                return moves;
            case PieceType::BISHOP:
                PseudoLegalMovesGenerator::getBishopMoves(color, board, moves);
                return moves;
            case PieceType::ROOK:
                PseudoLegalMovesGenerator::getRookMoves(color, board, moves);
                return moves;
            case PieceType::QUEEN:
                PseudoLegalMovesGenerator::getQueenMoves(color, board, moves);
                return moves;
            case PieceType::KING:
                PseudoLegalMovesGenerator::getKingMovesWithoutCastle(color, board, moves);
                PseudoLegalMovesGenerator::getCastles(color, board, moves);
                return moves;
            default:
                return moves;
        }
    }
};
