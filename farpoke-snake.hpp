#pragma once

#include "snake-base.hpp"

class FarpokeSnake : public SnakeBase
{
public:
    FarpokeSnake();

    Move getNextMove(Map& map) override;

private:

};
