#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <utility>

#include "../../Board/Board.hpp"
#include "../PiecesAssets/Assets.hpp"

class Piece {
public:
    explicit Piece(
        const PieceColor &color = PieceColor::WHITE,
        const PieceType &type = PieceType::PAWN,
        const sf::Texture& texture = whitePawnTexture
    ) : color(color), pieceType(type), sprite(texture) {
    }

    PieceColor color;
    PieceType pieceType;
    sf::Sprite sprite;
};
