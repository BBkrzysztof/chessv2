#include <iostream>
#include <thread>

#include "Bitboard.h"
#include "Engine/Engine.hpp"
#include "Engine/Utils/SearchConfig.hpp"
#include  "Parser/Parser.cpp"
#include "MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"
#include "Engine/AlphaBeta/AlphaBeta.hpp"


int main() {
    //const std::string fen = "rnbqkbn1/pppppppP/5rp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1"; // promo to queen
    //    const std::string fen = "rnb1k1n1/pppbpppP/4qrp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1"; // promo to queen gives mate
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);


    SearchConfig lim;
    lim.maxDepth = 12;
    lim.threads = std::thread::hardware_concurrency();
    lim.splitMinDepth = 6;
    lim.splitMinMoves = 0;

    TranspositionTable table{256};

    auto [score, bestMove] = Engine::run(board, lim, table);

    cout << score << " " << (int) Move::moveFrom(bestMove) << " " << (int) Move::moveTo(bestMove) << endl;


    return 0;
}
