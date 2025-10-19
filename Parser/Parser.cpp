#pragma once

#include <string>
#include <sstream>
#include <utility>

#include "utils/GameStateContainer.cpp"

using namespace std;

class Parser {
private:

    const int EN_PASSANT_COLUMN_START_POSITION = 2;

    int rowCounter = 0;
    int columnCounter = 0;

    string enp;
    string::iterator fenIterator;
    string::iterator fenIteratorEnd;

public:

    explicit Parser(string enp) {
        this->enp = std::move(enp);
        this->fenIterator = this->enp.begin();
        this->fenIteratorEnd = this->enp.end();
    }

    unique_ptr<GameStateContainer> parseFen() {

        auto gameStateContainer = std::make_unique<GameStateContainer>();
        auto rawPtr = gameStateContainer.get();

        this->parsePieces(rawPtr);
        this->parseGameState(rawPtr);
        this->parseEnPassant(rawPtr);
        this->parseMovesAndHalfMoves(rawPtr);

        return std::move(gameStateContainer);
    }

    static string encodeBoard(const GameStateContainer* container) {
        map<unsigned char, bitset<64>> parsedBoards = container->getBoards();

        int blancSpace = 0;

        stringstream fen;

        for (int i = 63; i >= 0; i--) {
            bool printed = false;

            for (const auto& element: parsedBoards) {
                if (element.second[i] != 0) {

                    if (blancSpace != 0) {
                        fen << blancSpace;
                        blancSpace = 0;
                    }
                    fen << element.first;
                    printed = true;
                    break;
                }
            }

            if (!printed) {
                blancSpace++;
            }

            if (i % 8 == 0 && i != 0) {
                if (blancSpace != 0) {
                    fen << blancSpace;
                    blancSpace = 0;
                }

                fen << "/";
            }
        }

        return fen.str();
    }

    static string encodeGameState(const string& encodedBoard, const GameStateContainer* container) {
        stringstream fen;

        fen << encodedBoard;
        char turn = container->isWhiteTurn() ? 'w' : 'b';

        fen << " " << turn << " ";

        if (container->getWhiteKingCastle()) {
            fen << "K";
        }
        if (container->getWhiteQueenCastle()) {
            fen << "Q";
        }
        if (container->getBlackKingCastle()) {
            fen << "k";
        }
        if (container->getBlackQueenCastle()) {
            fen << "q";
        }

        if (
                !container->getWhiteKingCastle() &&
                !container->getWhiteQueenCastle() &&
                !container->getBlackKingCastle() &&
                !container->getBlackQueenCastle()
                ) {
            fen << "-";
        }

        fen << " ";

        fen << container->getEnPassantMove() << " ";
        fen << container->getHalfMoveCounter() << " ";
        fen << container->getMoveCounter();

        return fen.str();
    }

private:

    void parsePieces(GameStateContainer* gameStateContainer) {
        while (this->fenIterator != this->fenIteratorEnd) {
            auto token = this->consume();

            if (isdigit(token)) {
                int number = ((int) token) - 48;
                if (number == 8) {
                    continue;
                }
                this->columnCounter += number;
                continue;
            }

            if (token == ' ') {
                break;
            }

            if (token == '/') {
                this->columnCounter = 0;
                this->rowCounter++;
                continue;
            }

            gameStateContainer->placePiece(token, this->rowCounter, this->columnCounter);

            this->columnCounter++;
        }
    }

    void parseGameState(GameStateContainer* gameStateContainer) {
        this->columnCounter = 0;

        while (this->fenIterator != this->fenIteratorEnd) {
            const auto token = this->consume();
            if (token == ' ') {
                this->columnCounter += 1;

                if (columnCounter == Parser::EN_PASSANT_COLUMN_START_POSITION) {
                    break;
                }

                continue;
            }

            switch (token) {
                case 'K':
                case 'Q':
                case 'k':
                case 'q':
                    gameStateContainer->updateCastles(token);
                    break;
                case 'w':
                    gameStateContainer->setWhiteTurn(true);
                    break;
                case 'b':
                    gameStateContainer->setWhiteTurn(false);
                    break;
                case '-':
                default:
                    continue;
            }


        }
    }

    void parseEnPassant(GameStateContainer* gameStateContainer) {
        this->columnCounter = 0;

        string buffer;

        while (this->fenIterator != this->fenIteratorEnd) {
            const auto token = this->consume();
            if (token == ' ') {
                break;
            }
            buffer += token;
        }

        gameStateContainer->setEnPassantMove(buffer);
    }

    void parseMovesAndHalfMoves(GameStateContainer* gameStateContainer) {
        this->columnCounter = 0;

        string buffer1, buffer2;

        while (this->fenIterator != this->fenIteratorEnd) {
            const auto token = this->consume();
            if (token == ' ') {
                break;
            }
            buffer1 += token;
        }

        while (this->fenIterator != this->fenIteratorEnd) {
            const auto token = this->consume();
            if (token == ' ') {
                break;
            }
            buffer2 += token;
        }

        gameStateContainer->setHalfMoveCounter(stoi(buffer1));
        gameStateContainer->setMoveCounter(stoi(buffer2));

    }

    unsigned char consume() {
        return *this->fenIterator++;
    }

};