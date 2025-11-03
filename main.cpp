#include <iostream>
#include <thread>

#include "Bitboard.h"
#include "Engine/Engine.hpp"
#include "Engine/Utils/SearchConfig.hpp"
#include  "Parser/Parser.cpp"
#include "MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"


int main() {
    const std::string fen = "rnbqkbn1/pppppppP/5rp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1";
    // const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);


    SearchConfig lim;
    lim.maxDepth = 8;
    lim.threads = std::thread::hardware_concurrency();
    lim.splitMinDepth = 6;
    lim.splitMinMoves = 6;

    auto res = Engine::run(board, lim);
    cout << res.score << endl;
    cout << (int) Move::moveFrom(res.bestMove) << " " << (int) Move::moveTo(res.bestMove) << endl;


    return 0;
}
