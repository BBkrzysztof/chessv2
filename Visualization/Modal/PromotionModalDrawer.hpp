#include <imgui.h>
#include <optional>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../../MoveGenerator/Move/Move.hpp"

static constexpr PieceType promoTypes[4] = {PieceType::QUEEN, PieceType::ROOK, PieceType::BISHOP, PieceType::KNIGHT};

static std::unordered_map<PieceType, sf::Texture> promoColors = {
    {PieceType::KNIGHT, Assets::whiteKnightTexture},
    {PieceType::BISHOP, Assets::whiteBishopTexture},
    {PieceType::ROOK, Assets::whiteRookTexture},
    {PieceType::QUEEN, Assets::whiteQueenTexture}
};


class PromotionModalDrawer {
public:
    static std::optional<Move::Move> drawModal(sf::RenderWindow &window, GameState &state) {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(
            ImVec2(window.getSize().x, window.getSize().y)
        );

        ImGui::SetNextWindowBgAlpha(0.0f);

        ImGui::Begin(
            "PromotionOverlay",
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        sf::RectangleShape overlay;

        overlay.setPosition({boardX - 10.f, boardY - 10.f});
        overlay.setSize({boardSize + 20.f, boardSize + 20.f});
        overlay.setFillColor(sf::Color(24, 24, 24, 128));

        const auto to = Move::moveTo(state.promotionMove.value());
        const auto toX = to % 8;


        sf::RectangleShape pickerContainer;

        pickerContainer.setPosition({boardX + (cellSize * toX), boardY});
        pickerContainer.setSize({cellSize, cellSize * 4.0f});
        pickerContainer.setFillColor(sf::Color(217, 217, 217, 175));


        window.draw(overlay);
        window.draw(pickerContainer);

        for (int i = 0; i < 4; i++) {
            sf::Sprite sprite{promoColors[promoTypes[i]]};
            sprite.setPosition({boardX + (cellSize * toX), boardY + (cellSize * i)});

            sf::CircleShape circle;
            circle.setRadius(cellSize * 0.45f);
            circle.setOrigin({circle.getRadius(), circle.getRadius()});
            circle.setPosition({boardX + (cellSize * toX), boardY + (cellSize * i)});
            circle.move({cellSize / 2, cellSize / 2});

            circle.setFillColor(sf::Color(217, 217, 217, 240));

            window.draw(circle);
            window.draw(sprite);

            ImGui::PushID(i);
            ImGui::SetCursorScreenPos({boardX + (cellSize * toX), boardY + (cellSize * i)});
            string id = "promo_" + std::to_string(i);

            if (ImGui::InvisibleButton(id.c_str(), {cellSize, cellSize})) {
                ImGui::PopID();
                ImGui::End();

                return encodeMove(state.promotionMove.value(), promoTypes[i]);
            }
            ImGui::PopID();
        }

        ImGui::End();

        return std::nullopt;
    }

private:
    static Move::Move encodeMove(const Move::Move &move, const PieceType type) {
        const auto from = Move::moveFrom(move);
        const auto to = Move::moveTo(move);

        return Move::encodeMove(from, to, Move::MoveType::MT_PROMOTION, static_cast<Move::Promo>(type - 1));
    }
};
