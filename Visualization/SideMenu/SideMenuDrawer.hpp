#pragma once

#include <iostream>
#include <imgui.h>

constexpr float V_SPACE_SMALL = 6.0f;
constexpr float V_SPACE_MED = 12.0f;
constexpr float V_SPACE_LARGE = 20.0f;

class SideMenuDrawer {
public:
    static void draw(GameState &state) {
        ImGui::SetNextWindowPos(ImVec2(750, 25));
        ImGui::SetNextWindowSize(
            ImVec2(320, boardSize)
        );

        ImGui::Begin(
            "Engine Control",
            nullptr,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoBackground |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoBringToFrontOnFocus
        );

        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 8.0f;
        style.FrameRounding = 6.0f;
        style.Colors[ImGuiCol_Button] = ImVec4(0.45f, 0.25f, 0.10f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.60f, 0.35f, 0.15f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.20f, 0.15f, 1.0f);


        ImGui::Text("FEN");
        ImGui::Dummy(ImVec2(0, V_SPACE_SMALL));

        InputTextStdString("##fen", state.fen);
        ImGui::SameLine();
        if (ImGui::Button("Apply")) {
            state.setNewBoard(Parser::loadFen(state.fen));
            ImGui::End();
            return;
        }

        ImGui::Dummy(ImVec2(0, V_SPACE_LARGE));

        ImGui::Spacing();
        ImGui::Text("Depth");
        ImGui::Dummy(ImVec2(0, V_SPACE_SMALL));
        ImGui::SliderInt("##depth", &state.lim.maxDepth,1,8);

        ImGui::Dummy(ImVec2(0, V_SPACE_LARGE));

        ImGui::Text("CPU's");
        ImGui::Dummy(ImVec2(0, V_SPACE_SMALL));

        ImGui::SliderInt("##cpu", &state.lim.threads,1,8);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, V_SPACE_LARGE));

        ImGui::Text("Control");
        ImGui::Dummy(ImVec2(0, V_SPACE_MED));

        ImGui::Dummy(ImVec2(0, V_SPACE_MED));
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.15f, 0.15f, 1.0f));
        if (ImGui::Button("Restart", ImVec2(220, 42))) {
            state.resetBoard();
        }

        ImGui::PopStyleColor();

        ImGui::Dummy(ImVec2(0, V_SPACE_LARGE));

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0, V_SPACE_LARGE));

        ImGui::Text("Logs");
        ImGui::Dummy(ImVec2(0, V_SPACE_MED));

        ImGui::BeginChild("logs", ImVec2(0, 150), true);
        // for (const auto &log: logs) {
        //     ImGui::TextUnformatted(log.c_str());
        // }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);
        ImGui::EndChild();


        ImGui::End();
    }

    static int InputTextCallback(ImGuiInputTextCallbackData *data) {
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
            auto *str = static_cast<std::string *>(data->UserData);
            str->resize(data->BufTextLen);
            data->Buf = str->data();
        }
        return 0;
    }

    static bool InputTextStdString(
        const char *label,
        std::string &value,
        ImGuiInputTextFlags flags = 0
    ) {
        flags |= ImGuiInputTextFlags_CallbackResize;

        return ImGui::InputText(
            label,
            value.data(),
            value.capacity() + 1,
            flags,
            InputTextCallback,
            &value
        );
    }
};
