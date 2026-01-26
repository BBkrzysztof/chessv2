#pragma once

#include <thread>
#include "../GameState/GameState.hpp"
#include "../../Engine/Engine.hpp"


class ArtificialPlayer {
public:
    static void run(GameState &state) {
        if (state.isAiPlayerRunning.exchange(true)) {
            return;
        }

        std::thread([&state]() {

            static TranspositionTable table{64};

            Board board(state.board);

            auto [score, bestMove] = Engine::run(board, state.lim, table);

            state.isAiPlayerRunning.store(false);
            state.moveOrder.emplace(bestMove);
        }).detach();
    }

    private:
};
