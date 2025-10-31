#include "../../Parser/Parser.cpp"
#include <catch2/catch_test_macros.hpp>

#include "../../MoveGenerator/MoveExecutor/MoveExecutor.hpp"

const auto precomputed = PreComputedMovesGenerator::generate();

TEST_CASE("test apply move (pawn d2->d4)", "[pawn d4]") {
    const std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w Qk - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(11, 27);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);

    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::PAWN]==0x800f700);
}

TEST_CASE("test capture move (pawn e4->d5)", "[pawn capture d5]") {
    const std::string fen = "rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(28, 35);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);


    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::PAWN]==0x80000ef00);
    REQUIRE(newBoard->pieces[PieceColor::BLACK][PieceType::PAWN]==0xf7000000000000);
}

TEST_CASE("test promotion move", "[promote pawn]") {
    const std::string fen = "rnbqkbn1/pppppppP/5rp1/8/8/3P4/PPP1PPP1/RNBQKBNR w Q - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(55, 63, Move::MoveType::MT_PROMOTION, Move::Promo::PR_QUEEN);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);


    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::PAWN]==0x87700);
    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::QUEEN]==0x8000000000000008);
}

TEST_CASE("test enpassant move", "[enpassant move]") {
    const std::string fen = "rnbqkbnr/pppppp1p/8/6pP/8/8/PPPPPPP1/RNBQKBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(39, 46, Move::MoveType::MT_ENPASSANT);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);


    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::PAWN]==0x400000007f00);
    REQUIRE(newBoard->pieces[PieceColor::BLACK][PieceType::PAWN]==0xbf000000000000);
}

TEST_CASE("test white short castle move", "[white short castle move]") {
    const std::string fen = "rnbqkbnr/pppppp1p/8/6pP/8/5BN1/PPPPPPP1/RNBQK2R w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(4, 6, Move::MoveType::MT_CASTLE);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);

    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::ROOK]==0x21);
    REQUIRE(newBoard->pieces[PieceColor::WHITE][PieceType::KING]==0x40);
}

TEST_CASE("test detect check", "[detect check]") {
    const std::string fen = "rnb1kbnr/pppppppp/4q3/8/8/8/PPPPQPPP/RNB1KBNR w KQkq - 0 1";
    const auto board = Parser::loadFen(fen);

    const auto move = Move::encodeMove(12, 3);
    const auto newBoard = MoveExecutor::executeMove(board, precomputed, move);

    Bitboards::print_bb(newBoard->occupancyAll);
    REQUIRE(newBoard->isCheck);

}
