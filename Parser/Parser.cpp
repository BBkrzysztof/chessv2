#pragma once

#include <string>

#include "../Board/Board.hpp"
#include "../Board/Zobrist.hpp"


class Parser {
public:
    static Board loadFen(const std::string_view fen) {
        using namespace std::literals;
        Board board;
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

        {
            const auto &pp = fields[0];
            int row = 7, col = 0;
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

        {
            char c = fields[1][0];
            if (c == 'w') board.side = PieceColor::WHITE;
            else if (c == 'b') board.side = PieceColor::BLACK;
        }

        {
            board.castle = 0;
            auto cs = fields[2];
            if (cs == "-"sv) {
            } else {
                for (const char &c: cs) {
                    switch (c) {
                        case 'K':
                            board.castle |= 1;
                            break;
                        case 'Q':
                            board.castle |= 2;
                            break;
                        case 'k':
                            board.castle |= 4;
                            break;
                        case 'q':
                            board.castle |= 8;
                            break;
                    }
                }
            }
        }

        {
            board.ep = parseEpSquare(fields[3]);
        }

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

    static std::string toFEN(const Board& b) {
        std::string fen;

        for (int rank = 7; rank >= 0; --rank) {
            int empty = 0;

            for (int file = 0; file < 8; ++file) {
                int sq = rank * 8 + file;
                int p = b.pieceOn[sq];

                if (p == -1) {
                    empty++;
                } else {
                    if (empty > 0) {
                        fen += char('0' + empty);
                        empty = 0;
                    }

                    PieceColor color = static_cast<PieceColor>(p / 6);
                    PieceType  type  = static_cast<PieceType>(p % 6);

                    fen += pieceToFenChar(color, type);
                }
            }

            if (empty > 0)
                fen += char('0' + empty);

            if (rank > 0)
                fen += '/';
        }

        fen += ' ';
        fen += (b.side == WHITE ? 'w' : 'b');

        fen += ' ';
        if (b.castle == 0) {
            fen += '-';
        } else {
            if (b.castle & 1) fen += 'K';
            if (b.castle & 2) fen += 'Q';
            if (b.castle & 4) fen += 'k';
            if (b.castle & 8) fen += 'q';
        }

        fen += ' ';
        if (b.ep == -1) {
            fen += '-';
        } else {
            char file = 'a' + (b.ep % 8);
            char rank = '1' + (b.ep / 8);
            fen += file;
            fen += rank;
        }

        fen += ' ';
        fen += std::to_string(b.halfMove);

        fen += ' ';
        fen += std::to_string(b.fullMove);

        return fen;
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

    static char pieceToFenChar(const PieceColor color, const PieceType type) {
        static const char table[6] = {'p', 'n', 'b', 'r', 'q', 'k'};
        const char c = table[type];
        return (color == WHITE) ? std::toupper(c) : c;
    }

    static void fenCharToPieceTypeAndColor(
        const char &c,
        PieceColor &col,
        PieceType &pc
    ) {
        switch (c) {
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
