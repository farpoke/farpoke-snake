#pragma once

#include <array>
#include <string>

enum class Move {
    Up, Down, Left, Right
};

constexpr std::array<Move, 4> ALL_MOVES = {
    Move::Left, Move::Up, Move::Right, Move::Down
};

std::string toString(Move move);
