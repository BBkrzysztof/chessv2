#pragma once
#include <SFML/Graphics/Texture.hpp>

#include "black_bishop_png.h"
#include "black_king_png.h"
#include "black_knight_png.h"
#include "black_pawn_png.h"
#include "black_queen_png.h"
#include "black_rook_png.h"
#include "white_bishop_png.h"
#include "white_king_png.h"
#include "white_knight_png.h"
#include "white_pawn_png.h"
#include "white_queen_png.h"
#include "white_rook_png.h"

namespace Assets {
    inline sf::Texture whitePawnTexture(white_pawn_png, white_pawn_png_len);
    inline sf::Texture whiteKnightTexture(white_knight_png, white_knight_png_len);
    inline sf::Texture whiteBishopTexture(white_bishop_png, white_bishop_png_len);
    inline sf::Texture whiteRookTexture(white_rook_png, white_rook_png_len);
    inline sf::Texture whiteQueenTexture(white_queen_png, white_queen_png_len);
    inline sf::Texture whiteKingTexture(white_king_png, white_king_png_len);

    inline sf::Texture blackPawnTexture(black_pawn_png, black_pawn_png_len);
    inline sf::Texture blackKnightTexture(black_knight_png, black_knight_png_len);
    inline sf::Texture blackBishopTexture(black_bishop_png, black_bishop_png_len);
    inline sf::Texture blackRookTexture(black_rook_png, black_rook_png_len);
    inline sf::Texture blackQueenTexture(black_queen_png, black_queen_png_len);
    inline sf::Texture blackKingTexture(black_king_png, black_king_png_len);
};
