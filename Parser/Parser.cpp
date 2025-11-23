#pragma once

#include <string>

#include "../Board/Board.hpp"
#include "../Board/Zobrist.hpp"


class Parser {
public:
    static Board loadFen(const std::string_view fen) {
        using namespace std::literals;
        Board board;


        // 1) Podziel na 6 pól FEN
        // [0] piece placement, [1] side, [2] castling, [3] ep, [4] halfmove, [5] fullmove
        std::string_view fields[6];
        {
            size_t i = 0, start = 0, f = 0;
            while (i <= fen.size() && f < 6) {
                if (i == fen.size() || fen[i] == ' ') {
                    if (i > start) fields[f++] = fen.substr(start, i - start);
                    start = i + 1;
                }
                ++i;
            }
        }

        // 3) Piece placement
        {
            const auto &pp = fields[0];
            int row = 7, col = 0; // zaczynamy od rank 8 (row=7), do 1 (row=0)
            for (size_t i = 0; i < pp.size(); ++i) {
                char c = pp[i];
                if (c == '/') {
                    --row;
                    col = 0;
                    continue;
                }
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    int empty = c - '0';
                    col += empty;
                    continue;
                }
                PieceColor pieceColor;
                PieceType pieceType;
                fenCharToPieceTypeAndColor(c, pieceColor, pieceType);

                const auto sq = static_cast<uint8_t>(row * 8 + col);
                board.setPiece(pieceColor, pieceType, sq);
                ++col;
            }
        }

        // 4) Side to move
        {
            char c = fields[1][0];
            if (c == 'w') board.side = PieceColor::WHITE;
            else if (c == 'b') board.side = PieceColor::BLACK;
        }

        // 5) Castling rights
        {
            board.castle = 0;
            auto cs = fields[2];
            if (cs == "-"sv) {
                // brak praw
            } else {
                for (const char &c: cs) {
                    switch (c) {
                        case 'K':
                            board.castle |= 1;
                            break; // white short
                        case 'Q':
                            board.castle |= 2;
                            break; // white long
                        case 'k':
                            board.castle |= 4;
                            break; // black short
                        case 'q':
                            board.castle |= 8;
                            break; // black long
                    }
                }
            }
        }

        // 6) En passant
        {
            board.ep = parseEpSquare(fields[3]);
        }

        // 7) Opcjonalne halfmove/fullmove
        if (!fields[4].empty()) {
            parseUint(fields[4], board.halfMove);
        } else board.halfMove = 0;

        if (!fields[5].empty()) {
            parseUint(fields[5], board.fullMove);
            if (board.fullMove < 1) board.fullMove = 1;
        } else board.fullMove = 1;

        board.zobrist = Zobrist::instance().computeKey(board);

        return board;
    }

private:
    static int fileCharToCol(const char &f) {
        if (f < 'a' || f > 'h') return -1;
        return static_cast<int>(f - 'a');
    }

    static int rankCharToRow(const char &r) {
        if (r < '1' || r > '8') return -1;
        return static_cast<int>(r - '1');
    }

    // Zwraca sq (0..63) lub -1 gdy brak („-”)
    static int parseEpSquare(const std::string_view tok) {
        if (tok == "-"sv) return -1;
        if (tok.size() != 2) return -1;
        int col = fileCharToCol(tok[0]);
        int row = rankCharToRow(tok[1]);
        if (col < 0 || row < 0) return -1;
        return row * 8 + col;
    }

    static void parseUint(const std::string_view tok, int &out) {
        int v = 0;
        for (char c: tok) {
            v = v * 10 + (c - '0');
        }
        out = v;
    }

    static void fenCharToPieceTypeAndColor(
        const char &c,
        PieceColor &col,
        PieceType &pc
    ) {
        switch (c) {
            // białe
            case 'P':
                col = PieceColor::WHITE;
                pc = PieceType::PAWN;
                break;
            case 'N':
                col = PieceColor::WHITE;
                pc = PieceType::KNIGHT;
                break;
            case 'B':
                col = PieceColor::WHITE;
                pc = PieceType::BISHOP;
                break;
            case 'R':
                col = PieceColor::WHITE;
                pc = PieceType::ROOK;
                break;
            case 'Q':
                col = PieceColor::WHITE;
                pc = PieceType::QUEEN;
                break;
            case 'K':
                col = PieceColor::WHITE;
                pc = PieceType::KING;
                break;
            // czarne
            case 'p':
                col = PieceColor::BLACK;
                pc = PieceType::PAWN;
                break;
            case 'n':
                col = PieceColor::BLACK;
                pc = PieceType::KNIGHT;
                break;
            case 'b':
                col = PieceColor::BLACK;
                pc = PieceType::BISHOP;
                break;
            case 'r':
                col = PieceColor::BLACK;
                pc = PieceType::ROOK;
                break;
            case 'q':
                col = PieceColor::BLACK;
                pc = PieceType::QUEEN;
                break;
            case 'k':
                col = PieceColor::BLACK;
                pc = PieceType::KING;
                break;
            default:
                break;
        }
    }
};
