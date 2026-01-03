#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include  "Parser/Parser.cpp"
#include "Visualization/BoardDrawer.hpp"
#include "Visualization/ArtificialPlayer/ArtificialPlayer.hpp"
#include "Visualization/GameState/GameStateDrawer.hpp"
#include "Visualization/Modal/GameStateModalDrawer.hpp"
#include "Visualization/Modal/PromotionModalDrawer.hpp"
#include "Visualization/SideMenu/SideMenuDrawer.hpp"


int main() {
    sf::RenderWindow window(
        sf::VideoMode({1152, 768}, 600),
        "Chess Engine UI",
        sf::Style::Default,
        sf::State::Windowed,
        sf::ContextSettings({0, 0, 8})
    );
    window.setFramerateLimit(30);
    ImGui::SFML::Init(window);
    window.setVisible(true);
    window.requestFocus();
    sf::Clock deltaClock;



    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    // const std::string fen = "rnb2knQ/pppbppp1/4qrp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 2";
    //const std::string fen = "rnb1k1n1/pppbpppP/4qrp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1";
    const auto board = Parser::loadFen(fen);
    GameState state{-1, board};

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event == std::nullopt) {
                continue;
            }

            if (event->is<sf::Event::Closed>()) {
                ImGui::SFML::Shutdown();
                window.close();
                return 0;
            }

            ImGui::SFML::ProcessEvent(window, event.value());
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        window.clear(bgColor);

        BoardDrawer::drawBoard(window);
        GameStateDrawer::draw(window, state);
        GameStateModalDrawer::parseGameState(state);
        state.isModalOpened = GameStateModalDrawer::drawModal(window, state);
        state.isModalOpened = state.isModalOpened || state.isPromotionModalOpen;

        SideMenuDrawer::draw(state);

        if (state.board.side == ARTIFICIAL_PLAYER_COLOR) {

            if (!state.isAiPlayerRunning && state.moveOrder != std::nullopt) {
                state.setNewBoard(MoveExecutor::executeMoveCopyMake(state.board, state.moveOrder.value()));

                ImGui::SFML::Render(window);
                window.display();
                continue;
            }

            ArtificialPlayer::run(state);
        }else {
            if (state.moveOrder != std::nullopt) {
                const auto moveType = Move::moveType(state.moveOrder.value());
                if (moveType == Move::MT_PROMOTION) {
                    state.promotionMove.emplace() = state.moveOrder.value();
                    state.isPromotionModalOpen = true;
                    state.isModalOpened = true;
                    state.moveOrder = std::nullopt;
                } else {
                    state.setNewBoard(MoveExecutor::executeMoveCopyMake(state.board, state.moveOrder.value()));
                }
            }

            if (state.isPromotionModalOpen && state.promotionMove != std::nullopt) {
                auto result = PromotionModalDrawer::drawModal(window, state);
                if (result != std::nullopt) {
                    state.setNewBoard(MoveExecutor::executeMoveCopyMake(state.board, result.value()));
                    state.isPromotionModalOpen = false;
                    state.isModalOpened = false;
                    state.promotionMove = std::nullopt;
                    state.moveOrder = std::nullopt;
                }
            }
        }

        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
