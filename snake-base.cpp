#include "snake-base.hpp"

#include "player-registry.hpp"

SnakeBase::SnakeBase(std::string name) :name(name) {}

void SnakeBase::snakeRegistered(std::string guid) {
    this->guid = guid;
    id = g_Registry.getId(guid);
}

void SnakeBase::gameStarting() {}
