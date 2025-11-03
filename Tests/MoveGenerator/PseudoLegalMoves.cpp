#include <catch2/catch_test_macros.hpp>

#include "../../Bitboard.h"
#include "../../Parser/Parser.cpp"
#include "../../MoveGenerator/PreComputedMoves/PreComputedMoves.hpp"
#include "../../MoveGenerator/PseudoLegalMovesGenerator/PseudoLegalMovesGenerator.hpp"

const auto precomputed = PreComputedMovesGenerator::generate();

TEST_CASE("Test all fields attacked by color", "[white][attacks]") {
    const std::string fen = "rrnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto result = PseudoLegalMovesGenerator::getFieldsAttackedByColor(
        PieceColor::WHITE,
        board
    );

    REQUIRE(result == 0x1aa44ffff7e);
}

TEST_CASE("Count all pseudo legal moves (from start position is 20 legal moves as white)", "[pseudo][legal][moves]") {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto result = PseudoLegalMovesGenerator::generatePseudoLegalMoves(
        board
    );

    REQUIRE(result.m.size() == 20);
}
