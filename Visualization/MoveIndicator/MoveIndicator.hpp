#pragma once
#include <SFML/Graphics/CircleShape.hpp>

#include "../../Board/Board.hpp"
#include "../BoardDrawer.hpp"

enum VisualizationMoveType {
    MT_NORMAL = 0,
    MT_CASTLE = 1,
    MT_ENPASSANT = 2,
    MT_PROMOTION = 3,
    MT_CAPTURE = 4,
};


class MoveIndicator {
public:
    MoveIndicator(
        const PieceColor pieceColor,
        const PieceType pieceType,
        const uint8_t position,
        const VisualizationMoveType moveType
    ) : pieceColor(pieceColor), pieceType(pieceType), position(position), moveType(moveType) {
        const auto positionVector = sf::Vector2f(
            boardX + cellSize * static_cast<float>(Bitboards::column_of(this->position)),
            boardY + cellSize * static_cast<float>(Bitboards::row_of(this->position))
        );

        this->selectionIndicator.setRadius(cellSize * 0.45f);
        this->setIndicatorColor(moveType);

        this->selectionIndicator.setOrigin({
            this->selectionIndicator.getRadius(),
            this->selectionIndicator.getRadius()
        });

        this->selectionIndicator.setPosition(positionVector);
        this->selectionIndicator.move({cellSize / 2, cellSize / 2});

    };

    sf::CircleShape getSelectionIndicator() const {
        return this->selectionIndicator;
    }

private:
    PieceColor pieceColor;
    PieceType pieceType;
    uint8_t position;
    VisualizationMoveType moveType;
    sf::CircleShape selectionIndicator;

    void setIndicatorColor(const VisualizationMoveType type) {
        switch (type) {
            case MT_NORMAL:
                this->selectionIndicator.setRadius(cellSize * 0.3f);
                selectionIndicator.setFillColor(indicatorGreen);
                break;
            case MT_ENPASSANT:
            case MT_CAPTURE:
                selectionIndicator.setFillColor(indicatorRed);
                break;
            case MT_CASTLE:
            case MT_PROMOTION:
                selectionIndicator.setFillColor(indicatorPurple);
                break;
        }
    }
};
