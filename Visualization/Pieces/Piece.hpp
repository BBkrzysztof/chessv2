#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "../../Board/Board.hpp"
#include "Assetss/Assets.hpp"

class Piece {
public:
    explicit Piece(
        const PieceColor &color = PieceColor::WHITE,
        const PieceType &type = PieceType::PAWN,
        const sf::Texture &texture = Assets::whitePawnTexture,
        const uint8_t &position = 0,
        const bool selected = false
    ) : color(color), pieceType(type), sprite(texture), position(position), selected(selected),
        selectionIndicator(std::nullopt) {
        const auto positionVector = sf::Vector2f(
            boardX + cellSize * static_cast<float>(Bitboards::column_of(this->position)),
            boardY + cellSize * static_cast<float>(Bitboards::row_of(this->position))
        );

        this->sprite.setPosition(positionVector);

        if (this->selected) {
            sf::CircleShape circle;
            circle.setRadius(cellSize * 0.45f);
            circle.setOrigin({circle.getRadius(), circle.getRadius()});
            circle.setPosition(positionVector);
            circle.move({cellSize / 2, cellSize / 2});

            circle.setFillColor(sf::Color(47, 85, 104,240));
            this->selectionIndicator.emplace(circle);
        }
    }

    PieceColor color;
    PieceType pieceType;
    sf::Sprite sprite;
    std::optional<sf::CircleShape> selectionIndicator;
    uint8_t position;
    bool selected;
};
