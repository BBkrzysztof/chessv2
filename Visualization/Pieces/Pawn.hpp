#include <SFML/Graphics/Sprite.hpp>

#include "../../Board/Board.hpp"
#include "../PiecesAssets/Assets.hpp"

class Pawn {
public:
    explicit Pawn(const PieceColor &color = PieceColor::WHITE) : sprite(whitePawnTexture) {
        this->color = color;
        this->sprite = sf::Sprite(whitePawnTexture);
        this->sprite.scale({0.8f, 0.8});
    }

    PieceColor color;
    sf::Sprite sprite;
};
