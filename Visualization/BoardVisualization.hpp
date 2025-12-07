#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Colors.hpp"
#include "../Board/Board.hpp"
#include "Pieces/Pawn.hpp"

class BoardVisualization {
public:
    //@todo remove static
    static void drawBoard(sf::RenderWindow &window) {
        const float boardX = 40.f;
        const float boardY = 40.f;
        const float boardSize = 640.f;
        const float cellSize = boardSize / 8.f;

        sf::RectangleShape boardBackground;
        boardBackground.setPosition({boardX - 10.f, boardY - 10.f});
        boardBackground.setSize({boardSize + 20.f, boardSize + 20.f});
        boardBackground.setFillColor(sf::Color(40, 30, 20));
        boardBackground.setOutlineThickness(4.f);
        boardBackground.setOutlineColor(boardBorder);

        std::vector<sf::RectangleShape> squares;
        squares.reserve(64);
        for (int rank = 0; rank < 8; ++rank) {
            for (int file = 0; file < 8; ++file) {
                sf::RectangleShape sq;
                sq.setSize({cellSize, cellSize});
                sq.setOutlineColor(sf::Color::Transparent);
                sq.setPosition({
                    boardX + static_cast<float>(file) * cellSize,
                    boardY + static_cast<float>(rank) * cellSize
                });
                bool isLight = ((rank + file) % 2 == 0);
                sq.setFillColor(isLight ? boardLight : boardDark);
                squares.push_back(sq);
            }
        }

        std::vector<Pawn> pawns;
        for (int file = 0; file < 8; ++file) {
            Pawn pawn(PieceColor::WHITE);
            float cx = boardX + file * cellSize ;
            float cy = boardY + 6 * cellSize + cellSize;
            pawn.sprite.setPosition({cx, cy});
            pawns.push_back(pawn);
        }

        window.draw(boardBackground);
        for (const auto &sq: squares) window.draw(sq);
        for (const auto &sq: pawns) window.draw(sq.sprite);
    }

private:
    Board currentBoard;
};
