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
        const uint8_t position = 0,
        const bool selected = false,
        const bool checked = false
    ) : color(color), pieceType(type), sprite(texture), selectionIndicator(std::nullopt), position(position),
        selected(selected) {
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

            circle.setFillColor(sf::Color(47, 85, 104, 240));
            this->selectionIndicator.emplace(circle);
        }

        this->checked = checked;
        if (this->checked) {
            sf::RectangleShape rect;
            rect.setSize({cellSize, cellSize});
            rect.setPosition(positionVector);

            rect.setFillColor(sf::Color(255, 84, 81, 150));
            this->checkIndicator.emplace(rect);
        }
    }

    PieceColor color;
    PieceType pieceType;
    sf::Sprite sprite;
    std::optional<sf::CircleShape> selectionIndicator;
    std::optional<sf::RectangleShape> checkIndicator;
    uint8_t position;
    bool selected;
    bool checked;
};
