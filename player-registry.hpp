#pragma once

#include <map>
#include <string>

class PlayerRegistry {
public:
    PlayerRegistry() = default;
    PlayerRegistry(const PlayerRegistry&) = delete;
    PlayerRegistry& operator= (const PlayerRegistry&) = delete;
    ~PlayerRegistry() = default;

    int getId(const std::string& guid);

private:
    std::map<std::string, int> players;
    int nextId = 1;
};

extern PlayerRegistry g_Registry;
