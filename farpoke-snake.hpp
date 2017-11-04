#pragma once

#include "snake-base.hpp"

class FarpokeSnake : public SnakeBase
{
public:
    FarpokeSnake();

    Move getNextMove(int tick, Map& map) override;

private:

};
