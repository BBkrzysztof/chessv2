#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "../Board/Board.hpp"
#include "Pieces/Piece.hpp"

class GameStateDrawer {
public:
    static void draw(sf::RenderWindow &window, const Board &board, int &selectedPiece) {
        std::vector<Piece> pieces;
        pieces.reserve(64);

        const auto pieceExtractor = [&pieces, &board, &window, &selectedPiece](
            const PieceColor &color,
            const PieceType &type,
            const sf::Texture &texture
        ) {
            for (BitBoard temp = board.pieces[color][type]; temp; Bitboards::pop_lsb(temp)) {
                const auto position = static_cast<uint8_t>(Bitboards::lsb_index(temp));
                // xor with 56 flips Bitboard to drawn board orientation
                auto piece = Piece(color, type, texture, position ^ 56, position == selectedPiece);
                pieces.push_back(piece);

                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    auto mousePosition = sf::Mouse::getPosition(window);
                    auto bounds = piece.sprite.getGlobalBounds();

                    if (bounds.contains(sf::Vector2<float>(mousePosition))) {
                        //@todo block selecting ai player side
                        if (board.side != piece.color) {
                            continue;
                        }

                        piece.selected = !piece.selected;
                        selectedPiece = position;
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
            if (const auto selectedIndicator = piece.selectionIndicator; selectedIndicator.has_value()) {
                window.draw(piece.selectionIndicator.value());
            }
            window.draw(piece.sprite);
        }
    }
};
