#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "../GameState/GameState.hpp"

struct ModalStyle {
    char *text{};
    ImVec4 color;
};

constexpr auto bg = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
constexpr auto green = ImVec4(0.35f, 1.00f, 0.35f, 1.00f);
constexpr auto white = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
constexpr auto brown = ImVec4(0.45f, 0.30f, 0.15f, 1.00f);
constexpr auto brownH = ImVec4(0.52f, 0.35f, 0.18f, 1.00f);
constexpr auto brownA = ImVec4(0.38f, 0.25f, 0.12f, 1.00f);
constexpr auto red = ImVec4(0.66f, 0.11f, 0.11f, 1.0f);


class GameStateModalDrawer {
public:
    static bool drawModal(sf::RenderWindow &window, GameState &state) {
        ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, ImVec4(0.f, 0.f, 0.f, 0.55f));

        ImGui::SetNextWindowSize(ImVec2(320, 360), ImGuiCond_Always);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));


        ImGui::PushStyleColor(ImGuiCol_WindowBg, bg);
        ImGui::PushStyleColor(ImGuiCol_Text, white);
        ImGui::PushStyleColor(ImGuiCol_Button, brown);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, brownH);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, brownA);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6.f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(26, 22));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(14, 12));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(12, 14));

        const auto style = parseStyle(state);
        ImGui::PushStyleColor(ImGuiCol_Border, style.color);

        const auto display = ImGui::BeginPopupModal("game-state-modal", nullptr, GameStateModalDrawer::flags);

        if (display) {
            ImDrawList *dl = ImGui::GetWindowDrawList();
            const ImVec2 p = ImGui::GetWindowPos();
            const ImVec2 s = ImGui::GetWindowSize();
            constexpr float thickness = 3.0f;
            constexpr float rounding = 6.0f;
            dl->AddRect(p, ImVec2(p.x + s.x, p.y + s.y), ImColor(style.color), rounding, 0, thickness);

            ImGui::SetWindowFontScale(1.5f);
            CenterText("Game over");
            ImGui::SetWindowFontScale(1.0f);

            ImGui::Dummy(ImVec2(0, 10));

            ImGui::PushStyleColor(ImGuiCol_Text, style.color);
            ImGui::SetWindowFontScale(1.15f);
            CenterText(style.text);
            ImGui::SetWindowFontScale(1.0f);
            ImGui::PopStyleColor();

            ImGui::Dummy(ImVec2(0, 40));

            const ImVec2 btnSize(ImGui::GetContentRegionAvail().x, 46);

            if (ImGui::Button("Restart", btnSize)) {
                state.resetBoard();
                ImGui::CloseCurrentPopup();
            }

            ImGui::Dummy(ImVec2(0, 10));

            if (ImGui::Button("Export game", btnSize)) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(6);
        ImGui::PopStyleColor(6);
        ImGui::PopStyleColor();

        return display;
    }

private:
    static void CenterText(const char *text) {
        const float windowWidth = ImGui::GetWindowSize().x;
        const float textWidth = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
        ImGui::TextUnformatted(text);
    }

    static ModalStyle parseStyle(const GameState &state) {
        ModalStyle style{};

        if (state.matColor != std::nullopt) {
            const auto color = state.matColor.value();
            ImGui::OpenPopup("game-state-modal");

            if (state.board.side == color) {
                style.color = red;
                style.text = "You lost";

                return style;
            }

            style.color = green;
            style.text = "You won";

            return style;
        }

        if (state.isStalemate) {
            ImGui::OpenPopup("game-state-modal");
            style.color = brownA;
            style.text = "Stalemate";

            return style;
        }
    }

    static inline ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings;
};
