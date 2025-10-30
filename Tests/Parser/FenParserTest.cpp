
#include "../../Parser/Parser.cpp"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("FEN parser", "[FEN parser]") {
    const std::string fen = "rnbqkbnr/8/1p1p1p1p/p1p1p1p1/P1P1P1P1/1P1P1P1P/8/RNBQKBNR w KQkq - 0 9";
    auto board = Parser::loadFen(fen);

    REQUIRE(board->pieces[PieceColor::WHITE][PieceType::PAWN] == 0x55aa0000);
    REQUIRE(board->pieces[PieceColor::BLACK][PieceType::PAWN] == 0xaa5500000000);

    REQUIRE(board->pieces[PieceColor::WHITE][PieceType::KNIGHT] == 0x42);
    REQUIRE(board->pieces[PieceColor::BLACK][PieceType::KNIGHT] == 0x4200000000000000);

    REQUIRE(board->pieces[PieceColor::WHITE][PieceType::BISHOP] == 0x24);
    REQUIRE(board->pieces[PieceColor::BLACK][PieceType::BISHOP] == 0x2400000000000000);

    REQUIRE(board->pieces[PieceColor::WHITE][PieceType::QUEEN] == 0x8);
    REQUIRE(board->pieces[PieceColor::BLACK][PieceType::QUEEN] == 0x800000000000000);

    REQUIRE(board->pieces[PieceColor::WHITE][PieceType::KING] == 0x10);
    REQUIRE(board->pieces[PieceColor::BLACK][PieceType::KING] == 0x1000000000000000);

    REQUIRE(board->ep == -1);
    REQUIRE(board->halfMove == 0);
    REQUIRE(board->fullMove == 9);
    REQUIRE(board->castle == 0xf);
}

TEST_CASE("Fen decode enpassant", "[Fen decode enpassant]") {
    const std::string fen = "rnbqkbnr/pp1ppp1p/2p5/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq g6 0 3";
    auto board = Parser::loadFen(fen);

    REQUIRE(board->ep == 46);
}

TEST_CASE("Decode castle rights", "[Decode castle rights]") {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Qk - 0 1";
    auto board = Parser::loadFen(fen);

    REQUIRE(board->castle == 6);
}
