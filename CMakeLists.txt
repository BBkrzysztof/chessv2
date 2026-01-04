cmake_minimum_required(VERSION 3.24)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(chess
        main.cpp
        MoveGenerator/PieceRelevantFieldsMask/RookRelevantMoveMask.hpp
        MoveGenerator/PieceRelevantFieldsMask/BishopRelevantMoveMask.hpp
        MoveGenerator/PieceAttacks/PawnAttack.hpp
        MoveGenerator/PieceAttacks/KingAttack.hpp
        MoveGenerator/PreComputedMoves/PreComputedMoves.hpp
        MoveGenerator/PreComputedMoves/MagicBoardIndexGenerator.hpp
        MoveGenerator/Move/Move.hpp
        Board/Board.hpp
        Parser/Parser.cpp
        MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp
        MoveGenerator/MoveExecutor/MoveExecutor.hpp
        Engine/Utils/SplitPoint.hpp
        Engine/ThreadPool/ThreadPool.hpp
        Engine/PvSplit/PvSplit.hpp
        Engine/Utils/SearchConfig.hpp
        Engine/Evaluation/Evaluation.hpp
        Engine/Engine.hpp
        Engine/TranspositionTable/TranspositionTable.hpp
        Board/Zobrist.hpp
        Engine/AlphaBeta/AlphaBeta.hpp
        MoveGenerator/MoveExecutor/UndoInfo.hpp
        Visualization/BoardDrawer.hpp
        Visualization/Colors.hpp
        Visualization/Pieces/Piece.hpp
        Visualization/Pieces/Assetss/Assets.hpp
        Visualization/GameState/GameStateDrawer.hpp
        Visualization/MovesGenerator/MovesGenerator.hpp
        Visualization/MoveIndicator/MoveIndicator.hpp
        Visualization/Modal/GameStateModalDrawer.hpp
        Visualization/Modal/PromotionModalDrawer.hpp
        Visualization/SideMenu/SideMenuDrawer.hpp
        Visualization/ArtificialPlayer/ArtificialPlayer.hpp
)

target_link_libraries(chess
        PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::Audio
        SFML::System
        ImGui-SFML::ImGui-SFML
)
