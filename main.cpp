#include "Parser/Parser.cpp"
#include "Utils.cpp"

using namespace std;

int main() {
    string fen = "rnb1kbnr/ppp2ppp/8/3pp3/4P3/P7/1PPPqPPP/RNBQK1NR w KQkq c6 0 1";
    string fen2 = "rnb1kbnr/1pp2ppp/1P6/p2pp3/4P3/P7/2PPqPPP/RNBQK1NR w KQkq - 0 1";
    string fen3 = "r1bqkb1r/pppp1ppp/2n5/8/2Bpn3/2P2N2/PP3PPP/RNBQK2R w KQkq - 0 6";
    auto parser = new Parser(fen3);
    auto container = parser->parseFen();

    auto encodedBoard = Parser::encodeBoard(container.get());
    auto parsedFen = Parser::encodeGameState(encodedBoard, container.get());

    Utils::displayBoard(container.get());

    return 0;
}