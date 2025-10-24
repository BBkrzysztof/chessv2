cmake_minimum_required(VERSION 3.24)
project(chess)

set(CMAKE_CXX_STANDARD 14)

add_executable(chess
        temp_main.cpp
        MoveGenerator/PieceMoveMask/RookMoveMask.hpp
        MoveGenerator/PieceMoveMask/BishopMoveMask.hpp)

target_compile_options(chess PRIVATE -O3 -march=native -mbmi2)
