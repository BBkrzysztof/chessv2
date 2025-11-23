// #pragma once
// #include <memory>
//
// #include "../../Board/Board.hpp"
// #include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"
// #include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"
// #include "../AlphaBeta/AlphaBeta.hpp"
// #include "../Evaluation/Evaluation.hpp"
// #include "../QuickSearch/QuickSearch.hpp"
// #include "../TranspositionTable/TranspositionTable.hpp"
// #include "../TranspositionTable/Zobrist.hpp"
//
// class PvSearch {
// public:
//     static int search(
//         const Board &board,
//         int alpha,
//         const int beta,
//         const int &depth,
//         const int ply
//     ) {
//         if (depth == 0) {
//             return Evaluation::evaluate(board);
//         }
//
//
//         const auto [m] = PseudoLegalMovesGenerator::generatePseudoLegalMoves(board);
//         const auto firstMove = m[0];
//
//         auto firstChild = MoveExecutor::executeMove(board, firstMove);
//         const auto score = -search(firstChild, -beta, -alpha, depth - 1, ply + 1);
//
//         if (score > beta) {
//             return beta;
//         }
//         if (score > alpha) {
//             alpha = score;
//         }
//
//         const auto canSplit = depth
//
//         for (int i = 1; i < m.size(); i++) {
//         }
//
//         return alpha;
//     }
// };
