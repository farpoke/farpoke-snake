#include "move.hpp"

std::string toString(Move move) {
    switch (move) {
    case Move::Up: return "UP";
    case Move::Down: return "DOWN";
    case Move::Left: return "LEFT";
    case Move::Right: return "RIGHT";
    default:
        throw std::logic_error("invalid move");
    }
}
