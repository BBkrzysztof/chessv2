#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "GameState.hpp"
#include "../../Board/Board.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
#include "../MovesGenerator/MovesGenerator.hpp"
#include "../Pieces/Piece.hpp"

class GameStateDrawer {
public:
    static void draw(sf::RenderWindow &window, GameState &state) {
        std::vector<Piece> pieces;
        pieces.reserve(64);

        const auto pieceExtractor = [&pieces, &window, &state](
            const PieceColor &color,
            const PieceType &type,
            const sf::Texture &texture
        ) {
            for (BitBoard temp = state.board.pieces[color][type]; temp; Bitboards::pop_lsb(temp)) {
                const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                // xor with 56 flips Bitboard to drawn board orientation
                auto piece = Piece(
                    color,
                    type,
                    texture,
                    position ^ 56,
                    position == state.selectedPiece,
                    type == PieceType::KING && state.checkedColor == color
                );

                pieces.push_back(piece);

                if (!state.isModalOpened && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    auto mousePosition = sf::Mouse::getPosition(window);
                    auto bounds = piece.sprite.getGlobalBounds();

                    if (bounds.contains(sf::Vector2<float>(mousePosition))) {
                        if (state.board.side != piece.color) {
                            continue;
                        }

                        piece.selected = !piece.selected;
                        state.selectedPiece = position;
                    }
                }
            }
        };

        pieceExtractor(PieceColor::WHITE, PieceType::PAWN, Assets::whitePawnTexture);
        pieceExtractor(PieceColor::WHITE, PieceType::KNIGHT, Assets::whiteKnightTexture);
        pieceExtractor(PieceColor::WHITE, PieceType::BISHOP, Assets::whiteBishopTexture);
        pieceExtractor(PieceColor::WHITE, PieceType::ROOK, Assets::whiteRookTexture);
        pieceExtractor(PieceColor::WHITE, PieceType::QUEEN, Assets::whiteQueenTexture);
        pieceExtractor(PieceColor::WHITE, PieceType::KING, Assets::whiteKingTexture);

        pieceExtractor(PieceColor::BLACK, PieceType::PAWN, Assets::blackPawnTexture);
        pieceExtractor(PieceColor::BLACK, PieceType::KNIGHT, Assets::blackKnightTexture);
        pieceExtractor(PieceColor::BLACK, PieceType::BISHOP, Assets::blackBishopTexture);
        pieceExtractor(PieceColor::BLACK, PieceType::ROOK, Assets::blackRookTexture);
        pieceExtractor(PieceColor::BLACK, PieceType::QUEEN, Assets::blackQueenTexture);
        pieceExtractor(PieceColor::BLACK, PieceType::KING, Assets::blackKingTexture);

        for (const auto &piece: pieces) {
            if (const auto checkIndicator = piece.checkIndicator; checkIndicator.has_value()) {
                window.draw(piece.checkIndicator.value());
            }

            if (const auto selectedIndicator = piece.selectionIndicator; !state.isModalOpened && selectedIndicator.has_value()) {
                Move::MoveList moves;
                moves.m.reserve(128);

                const auto parsedPosition = piece.position ^ 56;

                if (state.moves[parsedPosition].empty()) {
                    MovesGenerator::emit(piece.color, piece.pieceType, state.board, parsedPosition, state);
                }

                window.draw(piece.selectionIndicator.value());
                for (const auto &moveIndicators: state.moves[parsedPosition]) {
                    window.draw(moveIndicators.second.getSelectionIndicator());

                    if (!state.isModalOpened && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        auto mousePosition = sf::Mouse::getPosition(window);
                        auto bounds = moveIndicators.second.getSelectionIndicator().getGlobalBounds();

                        if (bounds.contains(sf::Vector2<float>(mousePosition))) {
                            if (piece.checked || state.board.side != piece.color) {
                                continue;
                            }
                            state.moveOrder = moveIndicators.first;
                        }
                    }
                }
            }
        }

        for (const auto &piece: pieces) {
            window.draw(piece.sprite);
        }
    }
};
