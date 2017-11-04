#include "player-registry.hpp"

PlayerRegistry g_Registry;

int PlayerRegistry::getId(const std::string& guid) {
    if (players.find(guid) == players.end())
        players[guid] = nextId++;
    return players[guid];
}
