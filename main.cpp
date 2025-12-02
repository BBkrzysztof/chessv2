#include <iostream>
#include <thread>

#include "Bitboard.h"
#include "Engine/Engine.hpp"
#include "Engine/Utils/SearchConfig.hpp"
#include  "Parser/Parser.cpp"
#include "MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"
#include "Engine/AlphaBeta/AlphaBeta.hpp"


int main() {
    // const std::string fen = "rnbqkbn1/pppppppP/5rp1/8/8/3P4/PPP1PPP1/RNBQKBNR b Q - 0 1"; // promo to queen
     // const std::string fen = "rnb1k1n1/pppbpppP/4qrp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1"; // promo to queen gives mate
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
     // const std::string fen = "rnb2knQ/pppbppp1/4qrp1/8/8/3P4/PPP1PPP1/RNBQKBNR b Q - 0 2";
    auto board = Parser::loadFen(fen);

    SearchConfig lim;
    lim.maxDepth = 8;
    lim.threads = 8;
    lim.splitMinDepth = 2;
    lim.splitMinMoves = 16;

    TranspositionTable table{128};

    auto [score, bestMove] = Engine::run(board, lim, table);

    // cout<<score<<endl;
    cout << score << " " << (int) Move::moveFrom(bestMove) << " " << (int) Move::moveTo(bestMove) << " " << (int)
            Move::movePromo(bestMove) << endl;

    // Bitboards::print_bb(Bitboards::bit(Move::moveFrom(bestMove)));
    // Bitboards::print_bb(Bitboards::bit(Move::moveTo(bestMove)));

    return 0;
}
